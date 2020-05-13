#pragma once

#include "TFInference.h"

// OpenCV headers
#include <opencv2/core.hpp>
// STL
#include <array>
#include <vector>
#include <tuple>
#include <iostream>

namespace jami 
{
	class PluginInference : public TensorflowInference 
	{
		public:
			/**
			 * @brief PluginInference
			 * Is a type of supervised learning where we detect objects in images
			 * Draw a bounding boxes around them
			 * @param model
			 */
			PluginInference(TFModel model);
			~PluginInference();

			std::vector<float> masksPredictions() const;

			/**
			 * @brief feedInput
			 * Checks if the image input dimensions matches the expected ones in the model
			 * If so, fills the image data directly to the model input pointer
			 * Otherwise, resizes the image in order to match the model expected image
			 * dimensions And fills the image data throught the resize method
			 * @param in: image data
			 * @param imageWidth
			 * @param imageHeight
			 * @param imageNbChannels
			 **/
			void feedInput(std::vector<uint8_t> &in, int imageWidth, int imageHeight,
							int imageNbChannels);
			/**
			 * @brief getInput
			 * Returns the input where to fill the data
			 * Use this method if you know what you are doing, all the necessary checks
			 * on dimensions must be done on your part
			 * @return std::tuple<uint8_t *, std::vector<int>>
			 * The first element in the tuple is the pointer to the storage location
			 * The second element is a dimensions vector that will helps you make
			 * The necessary checks to make your data size match the input one
			 */
			std::pair<uint8_t *, std::vector<int>> getInput();

			/**
			 * @brief setExpectedImageDimensions
			 * Sets imageWidth and imageHeight from the sources
			 */
			void setExpectedImageDimensions();

			// Getters
			int getImageWidth() const;
			int getImageHeight() const;
			int getImageNbChannels() const;

		private:
			int imageWidth = 0;
			int imageHeight = 0;
			int imageNbChannels = 0;
	};
} // namespace jami
