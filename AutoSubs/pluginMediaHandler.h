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

#pragma once

// Project
#include "audiosubscriber.h"
#include "messagequeue.h"
#include "videoSubscriber.h"

// Plugin
#include "plugin/jamiplugin.h"
#include "plugin/mediahandler.h"

using avSubjectPtr = std::weak_ptr<jami::Observable<AVFrame*>>;

namespace jami {

class MessageSender : public jami::Observer<AVFrame*> {
public:
    MessageSender(const JAMI_PluginAPI* api, MessageQueue* my_voice);
    void setCallId(std::string const& call_id);
    virtual void update(jami::Observable<AVFrame*>*, AVFrame* const&) override;
    virtual void attached(jami::Observable<AVFrame*>*) override;
    virtual void detached(jami::Observable<AVFrame*>*) override;

private:
    const JAMI_PluginAPI* api;
    std::string callId;
    MessageQueue* my_voice;
};

class PluginMediaHandler : public jami::CallMediaHandler {
public:
    PluginMediaHandler(
        const JAMI_PluginAPI* api,
        std::map<std::string, std::string>&& ppm, std::string&& dataPath,
        MessageQueue* my_voice, MessageQueue* incoming_subs);
    ~PluginMediaHandler() override;

    virtual void notifyAVFrameSubject(const StreamData& data, avSubjectPtr subject) override;
    virtual std::map<std::string, std::string> getCallMediaHandlerDetails() override;

    virtual void detach() override;
    virtual void setPreferenceAttribute(const std::string& key, const std::string& value) override;
    virtual bool preferenceMapHasKey(const std::string& key) override;

    std::string dataPath() const { return datapath_; }

private:
    const JAMI_PluginAPI* api;
    const std::string datapath_;
    std::map<std::string, std::string> ppm_;

    std::shared_ptr<VideoSubscriber> preview_vs, opponent_vs;
    std::shared_ptr<AudioSubscriber> audio_as;
    std::shared_ptr<MessageSender> msg_sender;

    MessageQueue* my_voice;
    MessageQueue* incoming_subs;
};
} // namespace jami
