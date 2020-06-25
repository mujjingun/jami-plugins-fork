/*
 *  Copyright (C) 2004-2020 Savoir-faire Linux Inc.
 *
 *  Author: Aline Gondim Santos <aline.gondimsantos@savoirfairelinux.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.
 */

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
