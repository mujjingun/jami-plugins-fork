#pragma once

//Project
#include "videoSubscriber.h"

// Plugin
#include "plugin/jamiplugin.h"
#include "plugin/mediahandler.h"

using avSubjectPtr = std::weak_ptr<jami::Observable<AVFrame*>>;

namespace jami 
{
	class PluginMediaHandler : public jami::CallMediaHandler 
	{
		public:
			PluginMediaHandler(std::map<std::string, std::string>&& ppm, std::string &&dataPath);
			~PluginMediaHandler() override;

			virtual void notifyAVFrameSubject(const StreamData &data, avSubjectPtr subject) override;
			virtual std::map<std::string, std::string> getCallMediaHandlerDetails() override;

			virtual void detach() override;
			virtual void setPreferenceAttribute(const std::string& key, const std::string& value) override;
			
			std::shared_ptr<VideoSubscriber> mpInput;
			std::shared_ptr<VideoSubscriber> mpReceive;
			
			std::string dataPath() const { return datapath_; }

		private:
        	bool preferenceMapHasKey(const std::string& key);

		private:
			const std::string datapath_;
			std::map<std::string, std::string> ppm_;
	};

}
