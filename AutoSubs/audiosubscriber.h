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

#include "model/asrmodel.h"
#include "messagequeue.h"

struct SwrContext;

namespace jami {

class AudioSubscriber : public jami::Observer<AVFrame*> {
public:
    AudioSubscriber(const std::string& dataPath, MessageQueue* queue);
    ~AudioSubscriber();

    virtual void update(jami::Observable<AVFrame*>*, AVFrame* const& iFrame) override;
    virtual void attached(jami::Observable<AVFrame*>* observable) override;
    virtual void detached(jami::Observable<AVFrame*>* observable) override;

    void detach();
    void stop();

private:
    // Observer pattern
    Observable<AVFrame*>* observable_ = nullptr;

    // ASR model
    ASRModel asrModel;

    // Threading
    std::thread processFrameThread;
    std::mutex inputLock;
    std::condition_variable inputCv;

    // Status variables of the processing
    bool running{true};
    bool initialFrame{true};
    bool newFrame{false};

    // for sample rate conversion
    SwrContext* swr_ctx = nullptr;
    std::uint8_t** dst_data = nullptr;
    std::int64_t max_dst_nb_samples;
    int dst_linesize;

    // buffer
    std::vector<std::int16_t> input_buffer;
};
}

#endif // AUDIOSUBSCRIBER_H
