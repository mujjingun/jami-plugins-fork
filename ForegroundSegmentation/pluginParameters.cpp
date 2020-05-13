#include "pluginParameters.h"// Logger
#include "pluglog.h"

PluginParameters pluginParameters;

void setGlobalPluginParameters(std::map<std::string, std::string> pp)
{
    Plog::log(Plog::LogPriority::INFO, "GLOBAL PARAMETERS", "IN");
    if (!pp.empty())
    {
        Plog::log(Plog::LogPriority::INFO, "GLOBAL PARAMETERS", "PP NOT EMPTY");
        if(pp.find("streamslist") != pp.end())
        {
            pluginParameters.stream = pp.at("streamslist");
            Plog::log(Plog::LogPriority::INFO, "GLOBAL STREAM ", pluginParameters.stream);
        }
        if(pp.find("modellist") != pp.end())
        {
            pluginParameters.model = pp.at("modellist");
            Plog::log(Plog::LogPriority::INFO, "GLOBAL MODEL ", pluginParameters.model);
        }
        if(pp.find("backgroundlist") != pp.end())
        {
            pluginParameters.image = pp.at("backgroundlist");
            Plog::log(Plog::LogPriority::INFO, "GLOBAL IMAGE ", pluginParameters.image);
        }
    }
}

void getGlobalPluginParameters(PluginParameters* mPluginParameters)
{
    mPluginParameters->image = pluginParameters.image;
    mPluginParameters->model = pluginParameters.model;
    mPluginParameters->stream = pluginParameters.stream;
}


PluginParameters* getGlobalPluginParameters()
{
    return &pluginParameters;
}