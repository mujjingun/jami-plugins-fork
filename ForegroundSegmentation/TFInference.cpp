#include "TFInference.h"
// Std libraries
#include <fstream>
#include <numeric>
#include <iostream>


#ifdef TFLITE 
    // Tensorflow headers
    #include <tensorflow/lite/interpreter.h>
    #include <tensorflow/lite/builtin_op_data.h>
    #include <tensorflow/lite/interpreter.h>
    #include <tensorflow/lite/kernels/register.h>
    #include <tensorflow/lite/model.h>
    #include <tensorflow/lite/optional_debug_tools.h>
#else
    #include <tensorflow/core/framework/graph.pb.h>
    #include <tensorflow/core/platform/env.h>
#endif //TFLITE

#include <pluglog.h>

const char sep = separator();
const std::string TAG = "FORESEG";

namespace jami
    {
    TensorflowInference::TensorflowInference(TFModel tfModel) : tfModel(tfModel) {}

    TensorflowInference::~TensorflowInference()
    { }

    bool TensorflowInference::isAllocated() const
    {
        return allocated;
    }

#ifdef TFLITE 

    void TensorflowInference::loadModel() 
    {
		Plog::log(Plog::LogPriority::INFO, TAG, "inside loadModel()");
        flatbufferModel = tflite::FlatBufferModel::BuildFromFile(tfModel.modelPath.c_str());
        if (!flatbufferModel) 
        {
            std::runtime_error("Failed to load the model file");
        }
        Plog::log(Plog::LogPriority::INFO, "TENSOR", "MODEL LOADED" );

    }
    void TensorflowInference::buildInterpreter() 
    {
		Plog::log(Plog::LogPriority::INFO, TAG, "inside buildInterpreter()");
        // Build the interpreter
        tflite::ops::builtin::BuiltinOpResolver resolver;
        tflite::InterpreterBuilder builder(*flatbufferModel, resolver);
        builder(&interpreter);
        if(interpreter) 
        {
            setInterpreterSettings();
            Plog::log(Plog::LogPriority::INFO, "TENSOR", "INTERPRETER BUILT" );

            if (tfModel.useNNAPI)
            {
                    TfLiteDelegate* optionalNnApiDelegate = tflite::NnApiDelegate();
                    // optionalNnApiDelegate = std::make_unique<TfLiteDelegate*>(tflite::NnApiDelegate());
                    
                    // if (interpreter->ModifyGraphWithDelegate(*(optionalNnApiDelegate.get())) != kTfLiteOk)
                    if (interpreter->ModifyGraphWithDelegate(optionalNnApiDelegate) != kTfLiteOk)
                    {
                        Plog::log(Plog::LogPriority::INFO, "TENSOR", "INTERPRETER ERROR!!!" );
                    }
                    else
                    {
                        Plog::log(Plog::LogPriority::INFO, "TENSOR", "INTERPRETER SET" );
                        allocateTensors();
                    }
            }
            else
            {
                allocateTensors();
            }
        }
    }

    void TensorflowInference::allocateTensors() 
    {
        if (interpreter->AllocateTensors() != kTfLiteOk) 
        {
            std::runtime_error("Failed to allocate tensors!");
        } else 
        {
            Plog::log(Plog::LogPriority::INFO, "TENSOR", "TENSORS ALLOCATED" );
            allocated = true;
        }
    }

    void TensorflowInference::describeModelTensors() const 
    {
        //PrintInterpreterState(interpreter.get());
        std::ostringstream oss;
        oss << "=============== inputs/outputs dimensions ==============="
                << "\n";
        const std::vector<int> inputs = interpreter->inputs();
        const std::vector<int> outputs = interpreter->outputs();
        oss << "number of inputs: " << inputs.size() << std::endl;
        oss << "number of outputs: " << outputs.size() << std::endl;

        Plog::log(Plog::LogPriority::INFO, "TENSOR", oss.str() );
        int input = interpreter->inputs()[0];
        int output = interpreter->outputs()[0];
        oss << "input 0 index: " << input << std::endl;
        oss << "output 0 index: " << output << std::endl;
        oss << "=============== input dimensions ==============="
                << std::endl;
        Plog::log(Plog::LogPriority::INFO, "TENSOR", oss.str() );
        // get input dimension from the input tensor metadata
        // assuming one input only

        for (size_t i = 0; i < inputs.size(); i++) 
        {
            std::stringstream ss;
            ss << "Input  " << i << "   ➛ ";
            describeTensor(ss.str(), interpreter->inputs()[i]);
        }
        oss.str("");
        oss << "=============== output dimensions ==============="
                << "\n";
        Plog::log(Plog::LogPriority::INFO, "TENSOR", oss.str() );
        // get input dimension from the input tensor metadata
        // assuming one input only
        for (size_t i = 0; i < outputs.size(); i++) 
        {
            std::stringstream ss;
            ss << "Output " << i << "   ➛ ";
            describeTensor(ss.str(), interpreter->outputs()[i]);
        }
    }

    void TensorflowInference::setInterpreterSettings() 
    {
        // interpreter->UseNNAPI(tfModel.useNNAPI);
        interpreter->SetAllowFp16PrecisionForFp32(tfModel.allowFp16PrecisionForFp32);

        interpreter->SetNumThreads(static_cast<int>(tfModel.numberOfThreads));
    }

    void TensorflowInference::describeTensor(std::string prefix, int index) const 
    {
        std::vector<int> dimensions = getTensorDimensions(index);
        size_t nbDimensions = dimensions.size();

        std::ostringstream tensorDescription;
        tensorDescription << prefix;
        for (size_t i = 0; i < nbDimensions; i++) 
        {
            if (i == dimensions.size() - 1)
            {
                tensorDescription << dimensions[i];
            } else 
            {
                tensorDescription << dimensions[i] << " x ";
            }
        }
        tensorDescription << std::endl;
        Plog::log(Plog::LogPriority::INFO, "TENSOR", tensorDescription.str() );
    }

    std::vector<int>
    TensorflowInference::getTensorDimensions(int index) const 
    {
        TfLiteIntArray *dims = interpreter->tensor(index)->dims;
        size_t size = static_cast<size_t>(interpreter->tensor(index)->dims->size);
        std::vector<int> result;
        result.reserve(size);
        for (size_t i = 0; i != size; i++) 
        {
            result.push_back(dims->data[i]);
        }

        dims = nullptr;

        return result;
    }

    void TensorflowInference::runGraph() 
    {
		Plog::log(Plog::LogPriority::INFO, TAG, "inside runGraph()");
        for (size_t i = 0; i < tfModel.numberOfRuns; i++) 
        {          
            if (interpreter->Invoke() != kTfLiteOk)                 
            {
                Plog::log(Plog::LogPriority::INFO, "RUN GRAPH", "A problem occured when running the graph");
            }
            else
            {
                Plog::log(Plog::LogPriority::INFO, "RUN GRAPH", "TF RUN OK");
            }
        }
    }

    void TensorflowInference::init() 
    {
        // Loading the model
        Plog::log(Plog::LogPriority::INFO, TAG, "inside init()" );   
        loadModel();
        buildInterpreter();
        describeModelTensors();
    }  

#else
    // Reads a model graph definition from disk, and creates a session object you
    // can use to run it.
    void TensorflowInference::LoadGraph()
    {
        tensorflow::GraphDef graph_def;
        tensorflow::Status load_graph_status = tensorflow::ReadBinaryProto(tensorflow::Env::Default(), tfModel.modelPath, &graph_def);
        if (!load_graph_status.ok()) {
            return ; //tensorflow::errors::NotFound("Failed to load compute graph at '",
                                                //tfModel.modelPath.c_str(), "'");
        }

        PluginParameters* parameters = getGlobalPluginParameters();

        tensorflow::SessionOptions options;
        if(parameters->useGPU)
        {
            options.config.mutable_gpu_options()->set_allow_growth(true);
            options.config.mutable_gpu_options()->set_per_process_gpu_memory_fraction(0.5);
        }
        (&session)->reset(tensorflow::NewSession(options));
        tensorflow::Status session_create_status = session->Create(graph_def);
        if (!session_create_status.ok()) {
            return ;
        }

        allocated = true;
    }

    void TensorflowInference::runGraph() 
    {
        for (size_t i = 0; i < tfModel.numberOfRuns; i++) 
        {
            // Actually run the image through the model.
            tensorflow::Status run_status = session->Run({{tfModel.inputLayer, imageTensor}}, {tfModel.outputLayer}, {}, &outputs);
            if (!run_status.ok())
            {
                Plog::log(Plog::LogPriority::INFO, "RUN GRAPH", "A problem occured when running the graph");
            }
            else
            {
                Plog::log(Plog::LogPriority::INFO, "RUN GRAPH", "TF RUN OK");
            }
        }
    }

    void TensorflowInference::init()
    {
        // Loading the model
        Plog::log(Plog::LogPriority::INFO, "TENSOR", "INSIDE THE INIT" );
        LoadGraph();
    }
#endif

}
