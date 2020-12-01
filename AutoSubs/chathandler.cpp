#include "chathandler.h"

#include <pluglog.h>

#include <sstream>

static const std::string TAG = "AutoSub";

namespace jami {

ChatHandler::ChatHandler(MessageQueue* incoming_subs)
    : incoming_msgs_handler(incoming_subs)
{
    Plog::log(Plog::LogPriority::INFO, TAG, "ChatHandler()");
}

ChatHandler::~ChatHandler()
{
}

void ChatHandler::notifyStrMapSubject(const bool direction, strMapSubjectPtr subject)
{
    std::ostringstream oss;
    oss << "notifyStrMapSubject " << (direction ? "in" : "out");
    Plog::log(Plog::LogPriority::INFO, TAG, oss.str());

    if (direction) {
        // incoming msgs
        subject->attach(&incoming_msgs_handler);
    }
}
}
