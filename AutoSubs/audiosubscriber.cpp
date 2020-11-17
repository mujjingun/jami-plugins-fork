#include "audiosubscriber.h"

// LOGGING
#include <pluglog.h>

static const std::string TAG = "AutoSub";

namespace jami {

AudioSubscriber::AudioSubscriber()
{
}

AudioSubscriber::~AudioSubscriber()
{
    std::ostringstream oss;
    oss << "~AudioSubscriber" << std::endl;
    stop();

    // join processing thread
    processFrameThread.join();
    Plog::log(Plog::LogPriority::INFO, TAG, oss.str());
}

void AudioSubscriber::update(jami::Observable<AVFrame*>*, AVFrame* const& iFrame)
{
    Plog::log(Plog::LogPriority::INFO, TAG, "Audio update");
}

void AudioSubscriber::attached(jami::Observable<AVFrame*>* observable)
{
    std::ostringstream oss;
    oss << "::Attached ! " << std::endl;
    Plog::log(Plog::LogPriority::INFO, TAG, oss.str());
    observable_ = observable;
}

void AudioSubscriber::detached(jami::Observable<AVFrame*>*)
{
    observable_ = nullptr;
    std::ostringstream oss;
    oss << "::Detached()" << std::endl;
    Plog::log(Plog::LogPriority::INFO, TAG, oss.str());
}

void AudioSubscriber::detach()
{
    if (observable_) {
        std::ostringstream oss;
        oss << "::Calling detach()" << std::endl;
        Plog::log(Plog::LogPriority::INFO, TAG, oss.str());
    }
}

void AudioSubscriber::stop()
{
    running = false;
    inputCv.notify_all();
}
}
