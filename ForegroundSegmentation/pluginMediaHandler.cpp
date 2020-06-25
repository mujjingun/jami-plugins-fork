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

#include "pluginMediaHandler.h"
// Logger
#include "pluglog.h"
const char sep = separator();
const std::string TAG = "FORESEG";

namespace jami 
{
	PluginMediaHandler::PluginMediaHandler(std::map<std::string, std::string>&& ppm, std::string &&datapath):
		datapath_{datapath}, ppm_{ppm}
	{
    	setGlobalPluginParameters(ppm_);
    	setId(datapath_);
		mVS = std::make_shared<VideoSubscriber>(datapath_);
		// mpInput = std::make_shared<VideoSubscriber>(datapath_);
		// mpReceive = std::make_shared<VideoSubscriber>(datapath_);
	}

	void PluginMediaHandler::notifyAVFrameSubject(const StreamData &data, jami::avSubjectPtr subject)
	{
		Plog::log(Plog::LogPriority::INFO, TAG, "IN AVFRAMESUBJECT");
		std::ostringstream oss;
		std::string direction = data.direction ? "Receive" : "Preview";
		oss << "NEW SUBJECT: [" << data.id << "," << direction << "]" << std::endl;


		bool preferredStreamDirection = false;
		if (!ppm_.empty() && ppm_.find("streamslist") != ppm_.end()) 
		{
			Plog::log(Plog::LogPriority::INFO, TAG, "SET PARAMETERS");
			// PluginParameters* mPluginParameters = nullptr; 
			// getGlobalPluginParameters(mPluginParameters);
			// Plog::log(Plog::LogPriority::INFO, TAG, "GOT PARAMETERS");
			preferredStreamDirection = ppm_.at("streamslist")=="in"?true:false;
			// if(mPluginParameters != nullptr)
			// {
				// preferredStreamDirection = mPluginParameters->stream=="in"?true:false;
			// }
		}
		oss << "preferredStreamDirection " << preferredStreamDirection << std::endl;
		if (data.type == StreamType::video && !data.direction && data.direction == preferredStreamDirection) 
		{
			subject->attach(mVS.get()); // my image
			// subject->attach(mpInput.get()); // my image
			oss << "got my sent image attached" << std::endl;
		} else if (data.type == StreamType::video && data.direction && data.direction == preferredStreamDirection) 
		{
			subject->attach(mVS.get()); // the image i receive from the others on the call
			// subject->attach(mpReceive.get()); // the image i receive from the others on the call
			oss << "got my received image attached" << std::endl;
		}
		Plog::log(Plog::LogPriority::INFO, TAG, oss.str());
	}

	std::map<std::string, std::string> PluginMediaHandler::getCallMediaHandlerDetails()
	{
		return {{"icoPath", datapath_ + sep + "icon.png"}};
	}

	void PluginMediaHandler::setPreferenceAttribute(const std::string &key, const std::string &value)
	{

	}

	bool PluginMediaHandler::preferenceMapHasKey(const std::string &key)
	{
		if (ppm_.find(key) == ppm_.end()) 
		{
			return false;
		}
		return true;
	}

	void PluginMediaHandler::detach()
	{
		mVS->detach();
		// mpInput->detach();
		// mpReceive->detach();
	}

	PluginMediaHandler::~PluginMediaHandler() 
	{
		std::ostringstream oss;
		oss << " ~FORESEG Plugin" << std::endl;
		Plog::log(Plog::LogPriority::INFO, TAG, oss.str());
		detach();
	}
}
