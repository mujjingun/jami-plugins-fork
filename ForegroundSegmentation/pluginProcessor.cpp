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
#ifndef __ANDROID__		
		backgroundImage.convertTo(backgroundImage, CV_32FC3);
#endif
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

void PluginProcessor::drawMaskOnReducedFrame(cv::Mat &frame,
		cv::Mat &frameReduced, std::vector<float>computedMask) 
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
        cv::Mat maskImg(pluginInference.getImageWidth(), pluginInference.getImageHeight(), 
							CV_32FC1, mFloatMask.data());

		cv::resize(maskImg, maskImg, cv::Size(maskImg.cols+2*absOFFSETX, maskImg.rows+2*absOFFSETY));

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
#ifndef __ANDROID__
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

		cv::Mat clone = frameReduced.clone();
		
		clone.convertTo(clone, roiMaskType);
		clone = clone.mul(roiMaskImg);
		clone += backgroundImage.mul(roiMaskImgComplementary);
		clone.convertTo(clone, origType);
		int numberChannels = 3;

		cv::resize(clone, clone, cv::Size(frame.cols, frame.rows));

		std::memcpy(frame.data, clone.data, 
		static_cast<size_t>(clone.cols) * static_cast<size_t>(clone.rows) * static_cast<size_t>(numberChannels) * sizeof(uint8_t));

#else
		for (int col = 0; col < frame.cols; col++)
		{
			for (int row = 0; row < frame.rows; row++)
			{
				float maskValue = maskImg.at<float>(cv::Point(col+absOFFSETX+OFFSETX, row+absOFFSETY+OFFSETY));
				frame.at<cv::Vec3b>(cv::Point(col, row)) = 
					backgroundImage.at<cv::Vec3b>(cv::Point(col, row)) * (1. - maskValue)
					+ frame.at<cv::Vec3b>(cv::Point(col, row)) * maskValue;
			}
		}
#endif // __ANDROID__
		computedMask3 = std::vector<float>(computedMask2.begin(), computedMask2.end());
		computedMask2 = std::vector<float>(computedMask1.begin(), computedMask1.end());
		computedMask1 = std::vector<float>(computedMask.begin(), computedMask.end());
	}


	void PluginProcessor::drawMaskOnFrame(
		cv::Mat &frame, std::vector<float>computedMask) 
	{
		// Plog::log(Plog::LogPriority::INFO, TAG, "inside drawMaskOnFrame()");
		if (computedMask.empty())
		{
			return;
		}
		
		scaleX = (float)(backgroundImage.cols) / (float)(pluginInference.getImageWidth());
		scaleY = (float)(backgroundImage.rows) / (float)(pluginInference.getImageHeight());
		int absOFFSETY = 4*scaleY;
		int absOFFSETX = 4*scaleX;
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
        cv::Mat maskImg(pluginInference.getImageWidth(), pluginInference.getImageHeight(), 
							CV_32FC1, mFloatMask.data());

		cv::resize(maskImg, maskImg, cv::Size(backgroundImage.cols+2*absOFFSETX, backgroundImage.rows+2*absOFFSETY));

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
#ifndef __ANDROID__
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

		
		int origType = frame.type();
		int roiMaskType = roiMaskImg.type();

		cv::Mat clone = frame.clone();
		
		clone.convertTo(clone, roiMaskType);
		clone = clone.mul(roiMaskImg);
		clone += backgroundImage.mul(roiMaskImgComplementary);
		clone.convertTo(clone, origType);
		int numberChannels = 3;
		std::memcpy(frame.data, clone.data, 
		static_cast<size_t>(clone.cols) * static_cast<size_t>(clone.rows) * static_cast<size_t>(numberChannels) * sizeof(uint8_t));

#else
		for (int col = 0; col < frame.cols; col++)
		{
			for (int row = 0; row < frame.rows; row++)
			{
				float maskValue = maskImg.at<float>(cv::Point(col+absOFFSETX+OFFSETX, row+absOFFSETY+OFFSETY));
				frame.at<cv::Vec3b>(cv::Point(col, row)) = 
					backgroundImage.at<cv::Vec3b>(cv::Point(col, row)) * (1. - maskValue)
					+ frame.at<cv::Vec3b>(cv::Point(col, row)) * maskValue;
			}
		}
#endif // __ANDROID__
		computedMask3 = std::vector<float>(computedMask2.begin(), computedMask2.end());
		computedMask2 = std::vector<float>(computedMask1.begin(), computedMask1.end());
		computedMask1 = std::vector<float>(computedMask.begin(), computedMask.end());
	}

} // namespace jami