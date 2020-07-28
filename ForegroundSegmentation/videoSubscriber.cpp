/*
 *  Copyright (C) 2004-2020 Savoir-faire Linux Inc.
 *
 *  Author: Aline Gondim Santos <aline.gondimsantos@savoirfairelinux.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.
 */

#include "videoSubscriber.h"
// Use for display rotation matrix
extern "C" {
#include <libavutil/display.h>
#include <accel.h>
}

// Opencv processing
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

// LOGGING
#include <pluglog.h>

const std::string TAG = "FORESEG";
const char sep = separator();

namespace jami
{
	VideoSubscriber::VideoSubscriber(const std::string &dataPath): path_{dataPath},
	pluginProcessor{dataPath}
	{
		/**
		 * Waits for new frames and then process them
		 * Writes the predictions in computedPredictions
		 **/
		processFrameThread = std::thread([this]
        {
            while (running)
            {
                std::unique_lock<std::mutex> l(inputLock);
                inputCv.wait(l, [this] { return not running or newFrame; });
                if (not running)
                {
                    break;
                }

				pluginProcessor.feedInput(fcopy.resizedFrameRGB);
                newFrame = false;
                /** Unclock the mutex, this way we let the other thread
                 *  copy new data while we are processing the old one
                 **/
                l.unlock();
				// Plog::log(Plog::LogPriority::INFO, TAG, "compute");
				pluginProcessor.computePredictions();
            }
        });
	}

	VideoSubscriber::~VideoSubscriber()
	{
		std::ostringstream oss;
		oss << "~MediaProcessor" << std::endl;
		stop();
		processFrameThread.join();
		Plog::log(Plog::LogPriority::INFO, TAG, oss.str());
	}

	void VideoSubscriber::update(jami::Observable<AVFrame *> *, AVFrame *const &iFrame)
	{
		if (isAttached)
		{
			AVFrame *pluginFrame = const_cast<AVFrame *>(iFrame);
			if (firstRun)
				pluginFrame = transferToMainMemory(incFrame, AV_PIX_FMT_NV12);
			//======================================================================================
			// GET FRAME ROTATION
			AVFrameSideData *side_data =
				av_frame_get_side_data(pluginFrame, AV_FRAME_DATA_DISPLAYMATRIX);

			int angle{0};
			if (side_data)
			{
				auto matrix_rotation = reinterpret_cast<int32_t *>(side_data->data);
				angle = static_cast<int>(av_display_rotation_get(matrix_rotation));
			}

			//======================================================================================
			// GET RAW FRAME
			// Use a non-const Frame
			// Convert input frame to BGR
			int inputHeight = pluginFrame->height;
			int inputWidth = pluginFrame->width;

			fcopy.originalSize = cv::Size{inputWidth, inputHeight};
            FrameUniquePtr bgrFrame = scaler.convertFormat(pluginFrame, AV_PIX_FMT_RGB24);
			cv::Mat frame =
				cv::Mat{bgrFrame->height, bgrFrame->width, CV_8UC3, bgrFrame->data[0],
						static_cast<size_t>(bgrFrame->linesize[0])};
			// First clone the frame as the original one is unusable because of
			// linespace

			cv::Mat clone = frame.clone();
			//======================================================================================

			pluginProcessor.setBackgroundRotation(angle);

			if (firstRun)
			{
				pluginProcessor.pluginInference.setExpectedImageDimensions();
				fcopy.resizedSize = cv::Size{pluginProcessor.pluginInference.getImageWidth(), pluginProcessor.pluginInference.getImageHeight()};

				cv::resize(clone, fcopy.resizedFrameRGB, fcopy.resizedSize);
				pluginProcessor.rotateFrame(angle, fcopy.resizedFrameRGB);

				cv::resize(pluginProcessor.backgroundImage, pluginProcessor.backgroundImage, fcopy.resizedSize);

				firstRun = false;
			}

            // auto process_start = std::chrono::system_clock::now();
			if (!newFrame)
			{
				std::lock_guard<std::mutex> l(inputLock);
				cv::resize(clone, fcopy.resizedFrameRGB, fcopy.resizedSize);
				pluginProcessor.rotateFrame(angle, fcopy.resizedFrameRGB);
				newFrame = true;
				inputCv.notify_all();
			}

			fcopy.predictionsFrameBGR = frame;
			fcopy.predictionsResizedFrameBGR = fcopy.resizedFrameRGB.clone();
			pluginProcessor.rotateFrame(-angle, fcopy.predictionsResizedFrameBGR);
			pluginProcessor.drawMaskOnFrame(fcopy.predictionsFrameBGR, fcopy.predictionsResizedFrameBGR,
												   pluginProcessor.computedMask, bgrFrame->linesize[0], angle);
			// pluginProcessor.printMask();

			//======================================================================================
			// REPLACE AVFRAME DATA WITH FRAME DATA
			if (bgrFrame && bgrFrame->data[0])
			{
				uint8_t* frameData = bgrFrame->data[0];
				if(angle == 90 || angle == -90)
				{
					// std::memmove(frameData, fcopy.predictionsFrameBGR.data, static_cast<size_t>(iFrame->width*iFrame->height*3) * sizeof(uint8_t));
					std::memmove(frameData, fcopy.predictionsFrameBGR.data, static_cast<size_t>(pluginFrame->width*pluginFrame->height*3) * sizeof(uint8_t));
				}
			}
			// Copy Frame meta data
			if (bgrFrame && pluginFrame)
			{
				av_frame_copy_props(bgrFrame.get(), pluginFrame);
				scaler.moveFrom(pluginFrame, bgrFrame.get());
			}

			// auto process_end = std::chrono::system_clock::now();
			// std::chrono::duration<double> processing_duration = process_end-process_start;

			// std::ostringstream oss;
			// oss << "Processing time: " << std::chrono::duration_cast<std::chrono::milliseconds>(processing_duration).count() << " ms\n";
			// Plog::log(Plog::LogPriority::INFO, TAG, oss.str());

			// Remove the pointer
			incFrame = nullptr;
		}
	}

	void VideoSubscriber::attached(jami::Observable<AVFrame *> *observable)
	{
		std::ostringstream oss;
		oss << "::Attached ! " << std::endl;
		Plog::log(Plog::LogPriority::INFO, TAG, oss.str());
		observable_ = observable;
		isAttached = true;
	}

	void VideoSubscriber::detached(jami::Observable<AVFrame *> *)
	{
		isAttached = false;
		observable_ = nullptr;
		std::ostringstream oss;
		oss << "::Detached()" << std::endl;
		Plog::log(Plog::LogPriority::INFO, TAG, oss.str());
	}

	void VideoSubscriber::detach()
	{
		if (isAttached)
		{
			std::ostringstream oss;
			oss << "::Calling detach()" << std::endl;
			Plog::log(Plog::LogPriority::INFO, TAG, oss.str());
			observable_->detach(this);
		}
	}

	void VideoSubscriber::stop()
	{
		running = false;
		inputCv.notify_all();
	}

	void VideoSubscriber::setBackground(const std::string& dataPath, const std::string& value)
	{
		pluginProcessor.setBackgroundImage(dataPath, value);
	}
}
