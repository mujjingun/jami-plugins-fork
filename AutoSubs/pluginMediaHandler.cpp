/**
 *  Copyright (C) 2020 Savoir-faire Linux Inc.
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA.
 */

#include "pluginMediaHandler.h"
#include "plugin/conversationhandler.h"
#include "pluginParameters.h"

// Logger
#include "pluglog.h"
static const std::string TAG = "AutoSub";

namespace jami {

PluginMediaHandler::PluginMediaHandler(
    const JAMI_PluginAPI* api, std::map<std::string, std::string>&& ppm,
    std::string&& datapath, MessageQueue* my_voice, MessageQueue* incoming_subs)
    : api(api)
    , datapath_{datapath}
    , ppm_{ppm}
    , my_voice(my_voice)
    , incoming_subs(incoming_subs)
{
    setGlobalPluginParameters(ppm_);
    setId(datapath_);
    preview_vs = std::make_shared<VideoSubscriber>(datapath_, my_voice);
    opponent_vs = std::make_shared<VideoSubscriber>(datapath_, incoming_subs);
    audio_as = std::make_shared<AudioSubscriber>(datapath_, my_voice);
    msg_sender = std::make_shared<MessageSender>(api, my_voice);
}

void PluginMediaHandler::notifyAVFrameSubject(const StreamData& data, jami::avSubjectPtr subject)
{
    Plog::log(Plog::LogPriority::INFO, TAG, "notifyAVFrameSubject  -- " + std::to_string(int(data.type)));
    if (data.type == StreamType::video) {
        if (!data.direction) {
            // my image
            Plog::log(Plog::LogPriority::INFO, TAG, "attach preview_vs: " + data.source + " " + data.id);
            subject->attach(preview_vs.get());

            // attach message sender
            msg_sender->setCallId(data.id);
            subject->attach(msg_sender.get());

        } else if (data.direction) {
            // the image I receive from the others on the call
            //Plog::log(Plog::LogPriority::INFO, TAG, "attach opponent_vs: " + data.source + " " + data.id);
            //subject->attach(opponent_vs.get());
        }
    } else if (data.type == StreamType::audio) {
        if (!data.direction) {
            // my audio
            subject->attach(audio_as.get());
        }
    }
}

std::map<std::string, std::string>
PluginMediaHandler::getCallMediaHandlerDetails()
{
    std::map<std::string, std::string> mediaHandlerDetails = {};
    mediaHandlerDetails["name"] = "Automatic Subtitles";
    mediaHandlerDetails["iconPath"] = datapath_ + separator() + "icon.png";
    mediaHandlerDetails["pluginId"] = id();

    return mediaHandlerDetails;
}

void PluginMediaHandler::setPreferenceAttribute(const std::string&, const std::string&)
{
}

bool PluginMediaHandler::preferenceMapHasKey(const std::string&)
{
    return false;
}

void PluginMediaHandler::detach()
{
    preview_vs->detach();
    opponent_vs->detach();
}

PluginMediaHandler::~PluginMediaHandler()
{
    std::ostringstream oss;
    oss << " ~AutoSub Plugin" << std::endl;
    Plog::log(Plog::LogPriority::INFO, TAG, oss.str());
    detach();
}

MessageSender::MessageSender(const JAMI_PluginAPI* api, MessageQueue* my_voice)
    : api(api)
    , my_voice(my_voice)
{
}

void MessageSender::setCallId(const std::string& call_id)
{
    callId = call_id;
}

void MessageSender::update(jami::Observable<AVFrame*>*, AVFrame* const&)
{
}

void MessageSender::attached(jami::Observable<AVFrame*>*)
{
    Plog::log(Plog::LogPriority::INFO, TAG, "MessageSender::attached");

    my_voice->addEventListener([this](std::string const& text) {
        std::map<std::string, std::string> content;
        content["text/plain"] = text;
        CallTextMessage msg{callId, content};
        api->invokeService(api, "sendCallTextMessage", &msg);
    });
}

void MessageSender::detached(jami::Observable<AVFrame*>*)
{
    my_voice->removeAllEventListeners();
}

} // namespace jami
