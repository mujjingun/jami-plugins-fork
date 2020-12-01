#ifndef CHATOBSERVER_H
#define CHATOBSERVER_H

#include <observer.h>

#include "messagequeue.h"
#include "plugin/conversationhandler.h"

namespace jami {
class ChatObserver : public jami::Observer<jami::ConvMsgPtr> {
public:
    ChatObserver(MessageQueue* queue);
    ~ChatObserver();

    void update(Observable<jami::ConvMsgPtr>*, const jami::ConvMsgPtr&) override;

    void attached(Observable<jami::ConvMsgPtr>*) override;
    void detached(Observable<jami::ConvMsgPtr>*) override;

private:
    Observable<jami::ConvMsgPtr>* observable_ = nullptr;
    MessageQueue* queue;
};
}

#endif // CHATOBSERVER_H
