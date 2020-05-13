#include "pluginInference.h"
// Std libraries
#include <cstring>
#include <numeric>
#include "pluglog.h"
// Tensorflow headers
#include "tensorflow/lite/interpreter.h"

namespace jami 
{
	PluginInference::PluginInference(TFModel model) : TensorflowInference(model) {	}

	PluginInference::~PluginInference(){}

	void PluginInference::feedInput(std::vector<uint8_t> &in, int imageWidth,
										int imageHeight, int imageNbChannels) 
	{
		auto input = getInput();
		std::vector<int> dims = input.second;
		// Relevant data starts from index 1, dims.at(0) = 1
		int expectedWidth = dims.at(1);
		int expectedHeight = dims.at(2);
		int expectedNbChannels = dims.at(3);

		if (imageNbChannels != expectedNbChannels) 
		{
			std::cerr << "The number of channels in the input should match the number "
						"of channels in the model";
		} else if (imageWidth != expectedWidth || imageHeight != expectedHeight) 
		{
			std::cerr << "The width and height of the input image doesn't match the "
						"expected width and height of the model";
		} else 
		{
			// Get the input pointer and feed it with data
			uint8_t *inputDataPointer = input.first;
			
			for (size_t i = 0; i < in.size(); i++) 
			{
				inputDataPointer[i] = in.at(i);
			}
			// Use of memcopy for performance
			std::memcpy(inputDataPointer, in.data(), in.size() * sizeof(uint8_t));
		}
	}

	std::pair<uint8_t *, std::vector<int>> PluginInference::getInput() 
	{
		// We assume that we have only one input
		// Get the input index
		int input = interpreter->inputs()[0];
		uint8_t *inputDataPointer = interpreter->typed_tensor<uint8_t>(input);
		// Get the input dimensions vector
		std::vector<int> dims = getTensorDimensions(input);

		return std::make_pair(inputDataPointer, dims);
	}

	// // Types returned by tensorflow
	// int type = interpreter->tensor(outputIndex)->type
	// typedef enum {
	// kTfLiteNoType = 0,
	// kTfLiteFloat32 = 1, float
	// kTfLiteInt32 = 2, int // int32_t
	// kTfLiteUInt8 = 3, uint8_t
	// kTfLiteInt64 = 4, int64_t
	// kTfLiteString = 5, 
	// kTfLiteBool = 6,
	// kTfLiteInt16 = 7, int16_t
	// kTfLiteComplex64 = 8,
	// kTfLiteInt8 = 9, int8_t
	// kTfLiteFloat16 = 10, float16_t
	// } TfLiteType;

	std::vector<float>
	PluginInference::masksPredictions() const 
	{
        int outputIndex = interpreter->outputs()[0];
        std::vector<int> dims = getTensorDimensions(outputIndex);
		int totalDimensions = 1;
		for (size_t i = 0; i < dims.size(); i++)
		{
			totalDimensions *= dims[i];
		}
		std::vector<float> out;
		
		int type = interpreter->tensor(outputIndex)->type;
		switch(type)
		{
			case 2:
			{
				int* outputDataPointer = interpreter->typed_tensor<int>(outputIndex);
				std::vector<int> output(outputDataPointer, outputDataPointer + totalDimensions); //when mod model
				out=std::vector<float>(output.begin(), output.end());
				break;
			}
			case 4:
			{
				int64_t* outputDataPointer = interpreter->typed_tensor<int64_t>(outputIndex);
				std::vector<int64_t> output(outputDataPointer, outputDataPointer + totalDimensions); //when orig model
				out=std::vector<float>(output.begin(), output.end());
				break;
			}
		}

        return out;
	}


	void PluginInference::setExpectedImageDimensions() 
	{
		// We assume that we have only one input
		// Get the input index
		int input = interpreter->inputs()[0];
		// Get the input dimensions vector
		std::vector<int> dims = getTensorDimensions(input);
		// Relevant data starts from index 1, dims.at(0) = 1
		imageWidth = dims.at(1);
		imageHeight = dims.at(2);
		imageNbChannels = dims.at(3);
	}

	int PluginInference::getImageWidth() const 
	{ 
		return imageWidth; 
	}

	int PluginInference::getImageHeight() const 
	{ 
		return imageHeight; 
	}

	int PluginInference::getImageNbChannels() const 
	{
		return imageNbChannels;
	}
} // namespace jami
