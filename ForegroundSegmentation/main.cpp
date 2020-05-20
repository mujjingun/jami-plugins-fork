#include <iostream>
#include <string.h>
#include <thread>
#include <memory>
#include <plugin/jamiplugin.h>
#include "pluginMediaHandler.h"


extern "C" 
{
    void pluginExit(void) { }

    JAMI_PluginExitFunc JAMI_dynPluginInit(const JAMI_PluginAPI *api)
    {
        std::cout << "**************************************" << std::endl << std::endl;
        std::cout << "**  FOREGROUND SEGMENTATION PLUGIN  **" << std::endl;
        std::cout << "**************************************" << std::endl << std::endl;

        // If invokeService doesn't return an error
        if(api) 
        {
            std::map<std::string, std::string> ppm;
            api->invokeService(api, "getPluginPreferences", &ppm);
            std::string dataPath;
            api->invokeService(api, "getPluginDataPath", &dataPath);
            auto fmp = std::make_unique<jami::PluginMediaHandler>(std::move(ppm), std::move(dataPath));

            if(!api->manageComponent(api,"CallMediaHandlerManager", fmp.release())) 
            {
                return pluginExit;
            }
        }
        return nullptr;
    }
}
