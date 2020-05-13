#include "pluginMediaHandler.h"
// Logger
#include "pluglog.h"
const char sep = separator();
const std::string TAG = "GENERIC";

namespace jami 
{
	PluginMediaHandler::PluginMediaHandler(std::map<std::string, std::string>&& ppm, std::string &&datapath):
		datapath_{datapath}, ppm_{ppm}
	{
    	setGlobalPluginParameters(ppm_);
    	setId(datapath_);
		mpInput = std::make_shared<VideoSubscriber>(datapath_);
		mpReceive = std::make_shared<VideoSubscriber>(datapath_);
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
			subject->attach(mpInput.get()); // my image
			oss << "got my sent image attached" << std::endl;
		} else if (data.type == StreamType::video && data.direction && data.direction == preferredStreamDirection) 
		{
			subject->attach(mpReceive.get()); // the image i receive from the others on the call
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
		mpInput->detach();
		mpReceive->detach();
	}

	PluginMediaHandler::~PluginMediaHandler() 
	{
		std::ostringstream oss;
		oss << " ~GENERIC Plugin" << std::endl;
		Plog::log(Plog::LogPriority::INFO, TAG, oss.str());
		detach();
	}
}
