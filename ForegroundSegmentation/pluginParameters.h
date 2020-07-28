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

#ifndef _PLUGINPARAMETERS_H_
#define _PLUGINPARAMETERS_H_

// #pragma once
#include <string>
#include <map>

struct PluginParameters {
    std::string stream = "out";
#ifdef TFLITE
    bool useGPU = false;
#ifdef __ANDROID
    std::string model = "model_256_Qlatency.tflite";
#else
    std::string model = "mModel.tflite";
#endif
#else
    #ifndef CPU
        bool useGPU = true;
    #else
        bool useGPU = false;
    #endif
    std::string model = "mModel.pb";
#endif //TFLITE
    std::string image = "background2.png";
};

void setGlobalPluginParameters(std::map<std::string, std::string> pp);

void getGlobalPluginParameters(PluginParameters* mPluginParameters);
PluginParameters* getGlobalPluginParameters();

#endif //__PLUGINPREFERENCE_H_