#pragma once

// Std libraries
#include <string>
#include <vector>
#include "pluginParameters.h"

struct TFModelConfiguration 
{
    TFModelConfiguration(std::string& model): modelPath{model} {}
    std::string modelPath;
    std::vector<unsigned int> normalizationValues;

    // Tensorflow specific settings

    #ifdef __ANDROID__
    bool useNNAPI = true;
    #else
    bool useNNAPI = false;
    #endif // __ANDROID__

    bool allowFp16PrecisionForFp32 = true;
    unsigned int numberOfThreads = 4;

    // User defined details
    bool inputFloating = false;
    unsigned int numberOfRuns = 1;
};

struct TFModel : TFModelConfiguration 
{
    TFModel(std::string&& model, std::string&& labels): TFModelConfiguration(model), labelsPath{labels}{}
    TFModel(std::string& model, std::string& labels): TFModelConfiguration(model), labelsPath{labels}{}
    TFModel(std::string&& model): TFModelConfiguration(model) {}
    TFModel(std::string& model): TFModelConfiguration(model) {}
    
    std::string labelsPath = " ";
    unsigned int labelsPadding = 16;
};
