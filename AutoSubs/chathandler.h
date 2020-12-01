#ifndef CHATHANDLER_H
#define CHATHANDLER_H

// Plugin
#include "plugin/conversationhandler.h"
#include "plugin/jamiplugin.h"

#include "chatobserver.h"
#include "messagequeue.h"

namespace jami {

class ChatHandler : public jami::ConversationHandler {
public:
    ChatHandler(MessageQueue* incoming_subs);
    ~ChatHandler();

    void notifyStrMapSubject(const bool direction, strMapSubjectPtr subject) override;

private:
    ChatObserver incoming_msgs_handler;
};
}

#endif // CHATHANDLER_H
