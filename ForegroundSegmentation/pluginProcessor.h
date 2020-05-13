#pragma once
// STL
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <map>
// Filters
#include "pluginInference.h"
// AvFrame
extern "C" {
#include <libavutil/frame.h>
}
// Plugin
#include "plugin/jamiplugin.h"
#include "plugin/mediahandler.h"
// Frame scaler for frame transformations
#include "framescaler.h"

namespace jami 
{
	class PluginProcessor 
	{
		public:
			PluginProcessor(const std::string &dataPath);			
			//~PluginProcessor();

			void initModel();
			/**
			 * @brief feedInput
			 * Takes a frame and feeds it to the model storage for predictions
			 * @param frame
			 */
			void feedInput(const cv::Mat &frame);

			/**
			 * @brief computePredictions
			 * Uses the model to compute the predictions and store them in
			 * computedPredictions
			 */
			void computePredictions();
						
			void printMask();
			void drawMaskOnFrame(
				cv::Mat &frame,
				const std::vector<float> computedMask);		

			// Output predictions
			std::vector<float> computedMask;
			std::vector<float> computedMask1;
			std::vector<float> computedMask2;
			std::vector<float> computedMask3;

            cv::Mat backgroundImage;	
			
			cv::Size kSize;
			float scaleX = 0;
			float scaleY = 0;	
			
            PluginInference pluginInference;
			std::string backgroundPath;			

		private:
            // Frame
            cv::Mat frame;
	};
} // namespace jami
