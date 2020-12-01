/**
 *  Copyright (C) 2020 Savoir-faire Linux Inc.
 *
 *  Author: Geon Park <mujjingun@gmail.com>
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
#include <memory>
#include <plugin/jamiplugin.h>
#include <string.h>
#include <thread>

#include "chathandler.h"
#include "messagequeue.h"
#include "pluginMediaHandler.h"

#include <pluglog.h>

#ifdef WIN32
#define EXPORT_PLUGIN __declspec(dllexport)
#else
#define EXPORT_PLUGIN
#endif

#define AutoSubs_VERSION_MAJOR 1
#define AutoSubs_VERSION_MINOR 0

static jami::MessageQueue* my_voice{};
static jami::MessageQueue* incoming_subs{};

extern "C" {
void pluginExit(void)
{
    Plog::log(Plog::LogPriority::INFO, "AutoSubs", "pluginExit");
    delete my_voice;
    delete incoming_subs;
}

EXPORT_PLUGIN JAMI_PluginExitFunc
JAMI_dynPluginInit(const JAMI_PluginAPI* api)
{
    std::cout << "**************************" << std::endl
              << std::endl;
    std::cout << "**    AutoSubs PLUGIN   **" << std::endl;
    std::cout << "**************************" << std::endl
              << std::endl;
    std::cout << " Version " << AutoSubs_VERSION_MAJOR << "." << AutoSubs_VERSION_MINOR
              << std::endl;
    std::cout << "Compilation time = " << __TIME__ << "\n";

    // If invokeService doesn't return an error
    if (api) {
        std::map<std::string, std::string> ppm;
        api->invokeService(api, "getPluginPreferences", &ppm);
        std::string dataPath;
        api->invokeService(api, "getPluginDataPath", &dataPath);

        my_voice = new jami::MessageQueue{};
        incoming_subs = new jami::MessageQueue{};

        auto chat = std::make_unique<jami::ChatHandler>(incoming_subs);
        api->manageComponent(api, "ConversationHandlerManager", chat.release());

        auto fmp = std::make_unique<jami::PluginMediaHandler>(api,
            std::move(ppm), std::move(dataPath), my_voice, incoming_subs);
        api->manageComponent(api, "CallMediaHandlerManager", fmp.release());

        return pluginExit;
    }
    return nullptr;
}
}
