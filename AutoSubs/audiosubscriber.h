#ifndef AUDIOSUBSCRIBER_H
#define AUDIOSUBSCRIBER_H

// AvFrame
extern "C" {
#include <libavutil/frame.h>
}
#include <observer.h>

// STl
#include <condition_variable>
#include <map>
#include <memory>
#include <thread>

namespace jami {

class AudioSubscriber : public jami::Observer<AVFrame*> {
public:
    AudioSubscriber();
    ~AudioSubscriber();

    virtual void update(jami::Observable<AVFrame*>*, AVFrame* const& iFrame) override;
    virtual void attached(jami::Observable<AVFrame*>* observable) override;
    virtual void detached(jami::Observable<AVFrame*>* observable) override;

    void detach();
    void stop();

private:
    // Observer pattern
    Observable<AVFrame*>* observable_ = nullptr;

    // Threading
    std::thread processFrameThread;
    std::mutex inputLock;
    std::condition_variable inputCv;

    // Status variables of the processing
    bool running{true};
    bool newFrame{false};
};
}

#endif // AUDIOSUBSCRIBER_H
