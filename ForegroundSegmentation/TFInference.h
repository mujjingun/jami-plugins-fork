#pragma once

// Library headers
#include "TFModels.h"
#include <tensorflow/lite/delegates/nnapi/nnapi_delegate.h>

// STL
#include <memory>
#include <string>
#include <vector>

namespace tflite 
{
    class FlatBufferModel;
    class Interpreter;
    class StatefulNnApiDelegate;
} // namespace tflite

namespace jami 
{
    class TensorflowInference 
    {
        public:
            /**
             * @brief TensorflowInference
             * Takes a supervised model where the model and labels files are defined
             * @param model
             */
            TensorflowInference(TFModel model);
            ~TensorflowInference();
            /**
             * @brief loadModel
             * Load the model from the file described in the Supervised Model
             */
            void loadModel();
            void buildInterpreter();
            void setInterpreterSettings();

            /**
             * @brief allocateTensors
             * Tries to allocate space for the tensors
             * In case of success isAllocated() should return true
             */
            void allocateTensors();

            /**
             * @brief runGraph
             * runs the underlaying graph model.numberOfRuns times
             * Where numberOfRuns is defined in the model
             */
            void runGraph();

            /**
             * @brief init
             * Inits the model, interpreter, allocates tensors and load the labels
             */
            void init();
            // Getters
            bool isAllocated() const;
            // Debug methods
            void describeModelTensors() const;
            void describeTensor(std::string prefix, int index) const;

        protected:

            /**
             * @brief getTensorDimensions
             * Utility method to get Tensorflow Tensor dimensions
             * Given the index of the tensor, the function gives back a vector
             * Where each element is the dimension of the vector-space (finite dimension)
             * Thus, vector.size() is the number of vector-space used by the tensor
             * @param index
             * @return
             */
            std::vector<int> getTensorDimensions(int index) const;

            TFModel tfModel;
            std::vector<std::string> labels;

            /**
             * @brief nbLabels
             * The real number of labels may not match the labels.size() because of padding
             */
            size_t nbLabels;

            // Tensorflow model and interpreter
            std::unique_ptr<tflite::FlatBufferModel> flatbufferModel;
            std::unique_ptr<tflite::Interpreter> interpreter;
            // std::unique_ptr<TfLiteDelegate*> optionalNnApiDelegate;

            // tflite::StatefulNnApiDelegate delegate = tflite::StatefulNnApiDelegate();
            bool allocated = false;
    };
}
