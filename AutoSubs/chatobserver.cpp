#include "chatobserver.h"

#include <sstream>

#include <pluglog.h>

static const std::string TAG = "AutoSub";

namespace jami {

ChatObserver::ChatObserver(MessageQueue* queue)
    : queue(queue)
{
}

ChatObserver::~ChatObserver()
{
}

void ChatObserver::update(Observable<ConvMsgPtr>*, const ConvMsgPtr& msg)
{
    if (msg->data_.count("text/plain")) {
        auto str = msg->data_["text/plain"];
        queue->setMessage(str);
        Plog::log(Plog::LogPriority::INFO, TAG, "Received: " + str);
    }

    Plog::log(Plog::LogPriority::INFO, TAG, "Author=" + msg->author_ + ", to=" + msg->to_);
}

void ChatObserver::attached(Observable<ConvMsgPtr>* observable)
{
    std::ostringstream oss;
    oss << "::Attached ! " << std::endl;
    Plog::log(Plog::LogPriority::INFO, TAG, oss.str());
    observable_ = observable;
}

void ChatObserver::detached(Observable<ConvMsgPtr>*)
{
    observable_ = nullptr;
    std::ostringstream oss;
    oss << "::Detached()" << std::endl;
    Plog::log(Plog::LogPriority::INFO, TAG, oss.str());
}
}
