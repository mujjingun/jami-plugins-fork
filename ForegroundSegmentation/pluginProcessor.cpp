#include "pluginProcessor.h"
// System includes
#include <cstring>
// OpenCV headers
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core.hpp>
// Logger
#include <pluglog.h>
// Avutil/Display for rotation

extern "C" {
#include <libavutil/display.h>
}

const char sep = separator();

const std::string TAG = "FORESEG";

PluginParameters* mPluginParameters = getGlobalPluginParameters(); 

namespace jami 
{

	PluginProcessor::PluginProcessor(const std::string &dataPath):
	pluginInference{TFModel{dataPath + sep + "models/" + mPluginParameters->model}},
	backgroundPath{dataPath + sep + "backgrounds" + sep + mPluginParameters->image}
	{
		initModel();
		backgroundImage = cv::imread(backgroundPath);
		if (backgroundImage.cols == 0)
		{
        	Plog::log(Plog::LogPriority::ERROR, TAG, "Background image not Loaded");
		}
		cv::cvtColor(backgroundImage, backgroundImage, cv::COLOR_BGR2RGB);
		backgroundImage.convertTo(backgroundImage, CV_32FC3);
		//TODO: properly resize the background image to maintain background aspect ratio in the output image;
        Plog::log(Plog::LogPriority::INFO, TAG, mPluginParameters->model);
	}

	void PluginProcessor::initModel()
	{
		Plog::log(Plog::LogPriority::INFO, TAG, "inside getImageNbChannels()");
		try {
			pluginInference.init();
		} catch (std::exception& e) 
		{
			Plog::log(Plog::LogPriority::ERROR, TAG, e.what());
		}
		std::ostringstream oss;
        oss << "Model is allocated " << pluginInference.isAllocated();
        Plog::log(Plog::LogPriority::INFO, TAG, oss.str());
	}


#ifdef TFLITE
	void PluginProcessor::feedInput(const cv::Mat &frame)
	{
		Plog::log(Plog::LogPriority::INFO, TAG, "inside feedInput()");
		auto pair = pluginInference.getInput();
		uint8_t *inputPointer = pair.first;
		// Relevant data starts from index 1, dims.at(0) = 1
		size_t imageWidth = static_cast<size_t>(pair.second[1]);
		size_t imageHeight = static_cast<size_t>(pair.second[2]);
		size_t imageNbChannels = static_cast<size_t>(pair.second[3]);
		std::memcpy(inputPointer, frame.data,
					imageWidth * imageHeight * imageNbChannels * sizeof(uint8_t));

		inputPointer = nullptr;
	}
#else
	void PluginProcessor::feedInput(const cv::Mat &frame) 
	{
		std::ostringstream oss;
		oss << frame.rows;
		Plog::log(Plog::LogPriority::INFO, TAG, oss.str());
		pluginInference.ReadTensorFromMat(frame);
	}
#endif //TFLITE

	void PluginProcessor::computePredictions() 
	{
		Plog::log(Plog::LogPriority::INFO, TAG, "inside computePredictions()");
		// Run the graph
		pluginInference.runGraph();
		auto predictions = pluginInference.masksPredictions();

		// Save the predictions
		computedMask = predictions;
	}

	void PluginProcessor::printMask()
	{
		for (size_t i = 0; i < computedMask.size(); i++)
		{
			// Log the predictions
			std::ostringstream oss;
			oss << "\nclass: "<< computedMask[i] << std::endl;
			Plog::log(Plog::LogPriority::INFO, TAG, oss.str());
		}
	}


	void copyByLine(uchar* frameData, uchar* applyMaskData, const int lineSize, cv::Size size)
	{
		if (3 * size.width == lineSize)
		{
			std::memcpy(frameData, applyMaskData, size.height * size.width * 3);;
		}
		else
		{
			int rows = size.height;
			int offset = 0;
			int maskoffset = 0;
			for (int i = 0; i < rows; i++)
			{
				std::memcpy(frameData + offset, applyMaskData + maskoffset, lineSize);
				offset += lineSize;
				maskoffset += 3 * size.width;
			}
		}
	}

void PluginProcessor::drawMaskOnFrame(cv::Mat &frame,
		cv::Mat &frameReduced, std::vector<float>computedMask, int lineSize)
	{
		// Plog::log(Plog::LogPriority::INFO, TAG, "inside drawMaskOnFrame()");
		if (computedMask.empty())
		{
			return;
		}
		//TODO: MAKE VARIABLE WITH THE MODEL not the platform
#ifdef __ANDROID__
		int absOFFSETY = 4;
		int absOFFSETX = 4;
#else
		int absOFFSETY = 8;
		int absOFFSETX = 8;
#endif		
		int OFFSETY = -absOFFSETY;
		int OFFSETX = -absOFFSETX;
		if (computedMask1.empty())
		{
			computedMask3 = std::vector<float>(computedMask.size(), 0);
			computedMask2 = std::vector<float>(computedMask.size(), 0);
			computedMask1 = std::vector<float>(computedMask.size(), 0);
		}

		std::vector<float> mFloatMask(computedMask.begin(), computedMask.end());
		for (size_t i = 0; i < computedMask.size(); i++)
		{
			if(computedMask[i] == 15)
			{
				computedMask[i] = 255;
				mFloatMask[i] = 255;
			}
			else
			{
				computedMask[i] = 0;
				#ifdef __ANDROID__
				mFloatMask[i] = (float)(   (int)((0.6 * computedMask1[i] + 0.3 * computedMask2[i] + 0.1 * computedMask3[i])) % 256   );
				#else
				mFloatMask[i] = 0.;
				#endif
			}
		}
        cv::Mat maskImg(pluginInference.getImageHeight(), pluginInference.getImageWidth(),
							CV_32FC1, mFloatMask.data());

		cv::resize(maskImg, maskImg, cv::Size(frameReduced.cols+2*absOFFSETX, frameReduced.rows+2*absOFFSETY));

		kSize = cv::Size(maskImg.cols*0.05, maskImg.rows*0.05);
		if(kSize.height%2 == 0)
		{
			kSize.height -= 1;
		}
		if(kSize.width%2 == 0)
		{
			kSize.width -= 1;
		}

		GaussianBlur (maskImg, maskImg, kSize, 0); //mask from 0 to 255.
		maskImg = maskImg / 255.; //mask from 0 to 1.
		cv::Mat applyMask = frameReduced.clone();

		cv::Rect roi(absOFFSETX+OFFSETX, absOFFSETY+OFFSETY, backgroundImage.cols, backgroundImage.rows); //Create a rect
		cv::Mat roiMaskImg = maskImg(roi); //Crop the region of interest using above rect

		cv::Mat roiMaskImgComplementary = 1. - roiMaskImg; //mask from 1. to 0

		std::vector<cv::Mat> channels;
		std::vector<cv::Mat> channelsComplementary;

		channels.emplace_back(roiMaskImg);
		channels.emplace_back(roiMaskImg);
		channels.emplace_back(roiMaskImg);
		channelsComplementary.emplace_back(roiMaskImgComplementary);
		channelsComplementary.emplace_back(roiMaskImgComplementary);
		channelsComplementary.emplace_back(roiMaskImgComplementary);

		cv::merge(channels, roiMaskImg);
		cv::merge(channelsComplementary, roiMaskImgComplementary);

		int origType = frameReduced.type();
		int roiMaskType = roiMaskImg.type();

		applyMask.convertTo(applyMask, roiMaskType);
		applyMask = applyMask.mul(roiMaskImg);
		applyMask += backgroundImage.mul(roiMaskImgComplementary);
		applyMask.convertTo(applyMask, origType);

		cv::resize(applyMask, applyMask, cv::Size(frame.cols, frame.rows));

		copyByLine(frame.data, applyMask.data, lineSize, cv::Size(frame.cols, frame.rows));
		computedMask3 = std::vector<float>(computedMask2.begin(), computedMask2.end());
		computedMask2 = std::vector<float>(computedMask1.begin(), computedMask1.end());
		computedMask1 = std::vector<float>(computedMask.begin(), computedMask.end());
	}
} // namespace jami