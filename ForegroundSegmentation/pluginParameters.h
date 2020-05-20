#ifndef _PLUGINPARAMETERS_H_
#define _PLUGINPARAMETERS_H_

// #pragma once
#include <string>
#include <map>

struct PluginParameters {
    std::string stream = "out";
#ifdef TFLITE
#ifdef __ANDROID
    std::string model = "model_256_Qlatency.tflite";
#else
    std::string model = "model_256_F_16.tflite";
#endif    
#else
    std::string model = "frozen_inference_graph.pb";
#endif //TFLITE
    std::string image = "background2.png";
};

void setGlobalPluginParameters(std::map<std::string, std::string> pp);

void getGlobalPluginParameters(PluginParameters* mPluginParameters);
PluginParameters* getGlobalPluginParameters();

#endif //__PLUGINPREFERENCE_H_