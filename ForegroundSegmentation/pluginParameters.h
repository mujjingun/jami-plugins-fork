#ifndef _PLUGINPARAMETERS_H_
#define _PLUGINPARAMETERS_H_

// #pragma once
#include <string>
#include <map>


struct PluginParameters {
    std::string stream = "out";
    std::string model = "model_256_Qlatency.tflite";
    std::string image = "background1.png";
};

void setGlobalPluginParameters(std::map<std::string, std::string> pp);

void getGlobalPluginParameters(PluginParameters* mPluginParameters);
PluginParameters* getGlobalPluginParameters();

#endif //__PLUGINPREFERENCE_H_