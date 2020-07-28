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
#include <plugin/jamiplugin.h>
#include <plugin/mediahandler.h>
// Frame scaler for frame transformations
#include <framescaler.h>

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
			void drawMaskOnFrame(cv::Mat &frame, cv::Mat &frameReduced, std::vector<float>computedMask, int lineSize, int angle);
			int getBackgroundRotation();
			void setBackgroundRotation(int angle);
			void setBackgroundImage(const std::string& dataPath, const std::string& value);
            void rotateFrame(int angle, cv::Mat &mat);

			// Output predictions
			std::vector<float> computedMask;

			cv::Mat previousMasks[2];
            cv::Mat backgroundImage;

			cv::Size kSize;
			float scaleX = 0;
			float scaleY = 0;

            PluginInference pluginInference;
			std::string backgroundPath;

		private:
            // Frame
            cv::Mat frame;
			int backgroundRotation = 0;
	};
} // namespace jami
