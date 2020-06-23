#include "pluginInference.h"
// Std libraries
#include <cstring>
#include <numeric>
#include "pluglog.h"

const char sep = separator();
const std::string TAG = "FORESEG";

namespace jami 
{
	PluginInference::PluginInference(TFModel model) : TensorflowInference(model)
	{
#ifndef TFLITE
		//Initialize TENSORFLOW_CC lib
		static const char* kFakeName = "fake program name";
		int argc = 1;
		char* fake_name_copy = strdup(kFakeName);
		char** argv = &fake_name_copy;
		tensorflow::port::InitMain(kFakeName, &argc, &argv);
		if (argc > 1) {
            Plog::log(Plog::LogPriority::INFO, "TENSORFLOW INIT", "Unknown argument " );
		}
		free(fake_name_copy);
#endif	//TFLITE
	}

	PluginInference::~PluginInference(){}



#ifdef TFLITE
	std::pair<uint8_t *, std::vector<int>> PluginInference::getInput() 
	{
		// Plog::log(Plog::LogPriority::INFO, TAG, "inside getInput()");
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
		// Plog::log(Plog::LogPriority::INFO, TAG, "inside masksPredictions()");
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
		// Plog::log(Plog::LogPriority::INFO, TAG, "inside setExpectedImageDimensions()");
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
#else //TFLITE
	// Given an image file name, read in the data, try to decode it as an image,
	// resize it to the requested size, and then scale the values as desired.
	void PluginInference::ReadTensorFromMat(const cv::Mat& image) 
	{
		// std::ostringstream oss;
		// oss << image.rows;
		// Plog::log(Plog::LogPriority::INFO, "ReadTensorFromMat", oss.str());
		tensorflow::StringPiece tmp_data = imageTensor.tensor_data();
		// oss << image.rows;
		// Plog::log(Plog::LogPriority::INFO, "ReadTensorFromMat", oss.str());
		memcpy(const_cast<char*>(tmp_data.data()), (image.data), image.rows * image.cols * sizeof(uint8_t));
	}

	std::vector<float>
	PluginInference::masksPredictions() const 
	{
		std::ostringstream oss;
		std::vector<int> dims;
		int flatSize = 1;
		int num_dimensions = outputs[0].shape().dims();
		// oss << num_dimensions;
		for(int ii_dim=0; ii_dim<num_dimensions; ii_dim++) {
			// oss << "  " << outputs[0].shape().dim_size(ii_dim);
			dims.push_back(outputs[0].shape().dim_size(ii_dim));
			flatSize *= outputs[0].shape().dim_size(ii_dim);
		}

		// oss << "  " << flatSize;
		// Plog::log(Plog::LogPriority::INFO, "masksPredictions", oss.str());
		std::vector<float> out;
		int type = outputs[0].dtype();

		// oss << "  " << type;
		// Plog::log(Plog::LogPriority::INFO, "masksPredictions", oss.str());

		switch(type)
		{
			case tensorflow::DataType::DT_INT32:
			{
				for (int offset = 0; offset < flatSize; offset++)
				{
					// Get vaule through .flat()
					out.push_back(static_cast<float> (outputs[0].flat<tensorflow::int32>()(offset)));
				}
				break;
			}
			case tensorflow::DataType::DT_INT64:
			{
				for (int offset = 0; offset < flatSize; offset++)
				{
					// Get vaule through .flat()
					// if (outputs[0].flat<tensorflow::int64>()(offset) == 15 or outputs[0].flat<tensorflow::int64>()(offset) == 1)
					// {
					// 	oss << "  " << outputs[0].flat<tensorflow::int64>()(offset);
					// 	Plog::log(Plog::LogPriority::INFO, "masksPredictions", oss.str());
					// }
					out.push_back(static_cast<float> (outputs[0].flat<tensorflow::int64>()(offset)));
				}
				break;
			}
			default:
			{
				for (int offset = 0; offset < flatSize; offset++)
				{
					// Get vaule through .flat()
					out.push_back(0);
				}
				break;
			}
		}

        return out;
	}

	void PluginInference::setExpectedImageDimensions()
	{

		if (tfModel.dims[1] != 0)
		{
			imageWidth = tfModel.dims[1];
		}
		if (tfModel.dims[2] != 0)
		{
			imageHeight = tfModel.dims[2];
		}
		if (tfModel.dims[3] != 0)
		{
			imageNbChannels = tfModel.dims[3];
		}
	}
#endif

	int PluginInference::getImageWidth() const
	{ 
		// Plog::log(Plog::LogPriority::INFO, TAG, "inside getImageWidth()");
		return imageWidth; 
	}

	int PluginInference::getImageHeight() const
	{ 
		// Plog::log(Plog::LogPriority::INFO, TAG, "inside getImageHeight()");
		return imageHeight;
	}

	int PluginInference::getImageNbChannels() const
	{
		// Plog::log(Plog::LogPriority::INFO, TAG, "inside getImageNbChannels()");
		return imageNbChannels;
	}
} // namespace jami
