#include "pluginProcessor.h"
// System includes
#include <cstring>
// OpenCV headers
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core.hpp>
// Logger
#include "pluglog.h"
// Avutil/Display for rotation

extern "C" {
#include <libavutil/display.h>
}

const char sep = separator();

const std::string TAG = "GENERIC";

PluginParameters* mPluginParameters = getGlobalPluginParameters(); 

namespace jami 
{

	PluginProcessor::PluginProcessor(const std::string &dataPath):
	pluginInference{TFModel{dataPath + sep + "models/" + mPluginParameters->model,
	// pluginInference{TFModel{dataPath + sep + "models/mobilenet_v2_deeplab_v3_256_myquant.tflite",
							dataPath + sep + "models/pascal_voc_labels_list.tflite"}},
	// backgroundPath{dataPath + sep + "backgrounds" + sep + "background1.png"}
	backgroundPath{dataPath + sep + "backgrounds" + sep + mPluginParameters->image}
	{
		initModel();
		backgroundImage = cv::imread(backgroundPath);
	}

	void PluginProcessor::initModel()
	{
		try {
			pluginInference.init();
		} catch (std::exception& e) 
		{
			Plog::log(Plog::LogPriority::ERROR, TAG, e.what());
		}
		std::ostringstream oss;
        oss << "Model is allocated " << pluginInference.isAllocated();
        Plog::log(Plog::LogPriority::INFO, "GENERIC", oss.str());
	}

	void PluginProcessor::feedInput(const cv::Mat &frame) 
	{
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

	void PluginProcessor::computePredictions() 
	{
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
			// for (int j = 0; j < computedMask.rows; j++) 
			{
				// Log the predictions
				std::ostringstream oss;
				// oss << "\nrows: " << computedMask.rows << std::endl;
				// oss << "\ncols: " << computedMask.cols << std::endl;
				// oss << "\nclass "<<i<<"x"<<j<<": " << computedMask.at<int>(cv::Point(i,j)) << std::endl;
				oss << "\nclass: "<< computedMask[i] << std::endl;
				Plog::log(Plog::LogPriority::INFO, TAG, oss.str());
			}
		}
	}

	void PluginProcessor::drawMaskOnFrame(
		cv::Mat &frame, std::vector<float>computedMask) 
	{
		scaleX = (float)(backgroundImage.cols) / (float)(pluginInference.getImageWidth());
		scaleY = (float)(backgroundImage.rows) / (float)(pluginInference.getImageHeight());
		int absOFFSETY = 8*scaleY;
		int absOFFSETX = 8*scaleX;
		int OFFSETY = -absOFFSETY;
		int OFFSETX = -absOFFSETX;
		if (computedMask1.size() == 0)
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
				mFloatMask[i] = (float)(   (int)((0.6 * computedMask1[i] + 0.3 * computedMask2[i] + 0.1 * computedMask3[i])) % 256   );
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

		GaussianBlur (maskImg, maskImg, kSize, 0);
		
		
		for (int col = 0; col < frame.cols; col++)
		{
			for (int row = 0; row < frame.rows; row++)
			{
				cv::Point point(col+absOFFSETX+OFFSETX, row+absOFFSETY+OFFSETY);
				float maskValue = maskImg.at<float>(point)/255.;
				frame.at<cv::Vec3b>(cv::Point(col, row)) = 
					backgroundImage.at<cv::Vec3b>(cv::Point(col, row)) * (1 - maskValue)
					+ frame.at<cv::Vec3b>(cv::Point(col, row)) * maskValue;
			}
		}

		computedMask3 = std::vector<float>(computedMask2.begin(), computedMask2.end());
		computedMask2 = std::vector<float>(computedMask1.begin(), computedMask1.end());
		computedMask1 = std::vector<float>(computedMask.begin(), computedMask.end());
	}

} // namespace jami
