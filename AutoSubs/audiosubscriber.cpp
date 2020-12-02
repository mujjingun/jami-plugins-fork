#include "audiosubscriber.h"

extern "C" {
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
}

// LOGGING
#include <pluglog.h>

#include <fstream>

static const std::string TAG = "AutoSub";

namespace jami {

AudioSubscriber::AudioSubscriber(const std::string& dataPath, MessageQueue* queue)
    : asrModel(dataPath + separator() + "model.zip")
{
    swr_ctx = swr_alloc();
    if (!swr_ctx) {
        Plog::log(Plog::LogPriority::ERR, TAG, "Could not allocate resampler context");
    } else {
        Plog::log(Plog::LogPriority::INFO, TAG, "Allocated resampler context successfully");
    }

    /**
     * Waits for new frames and then process them
     * Writes the predictions to queue
     **/
    processFrameThread = std::thread([queue, this] {
        std::vector<std::int16_t> input_copy;
        while (running) {
            std::unique_lock<std::mutex> l(inputLock);
            inputCv.wait(l, [this] { return !running || newFrame; });
            if (!running) {
                break;
            }

            // copy input
            std::swap(input_copy, input_buffer);
            input_buffer.clear();

            newFrame = false;
            l.unlock();

            // compute predictions
            auto output = asrModel.process(input_copy.data(), int(input_copy.size()));
            if (output.size() > 0) {
                queue->setMessage(output);
            }
        }
    });
}

AudioSubscriber::~AudioSubscriber()
{
    Plog::log(Plog::LogPriority::INFO, TAG, "~AudioSubscriber");

    stop();

    // join processing thread
    processFrameThread.join();

    // free resampler
    if (swr_ctx) {
        swr_free(&swr_ctx);
    }
    if (dst_data) {
        av_freep(&dst_data[0]);
        av_freep(dst_data);
    }
}

void AudioSubscriber::update(jami::Observable<AVFrame*>*, AVFrame* const& iFrame)
{
    /*
    Plog::log(Plog::LogPriority::INFO, TAG,
        "Sample rate = " + std::to_string(iFrame->sample_rate)
            + " num samples = " + std::to_string(iFrame->nb_samples)
            + " channels = " + std::to_string(iFrame->channels)
            + " channel layout = " + std::to_string(iFrame->channel_layout));
    */

    if (initialFrame) {
        initialFrame = false;

        av_opt_set_int(swr_ctx, "in_channel_layout", iFrame->channel_layout, 0);
        av_opt_set_int(swr_ctx, "in_sample_rate", iFrame->sample_rate, 0);
        av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", static_cast<AVSampleFormat>(iFrame->format), 0);

        av_opt_set_int(swr_ctx, "out_channel_layout", AV_CH_LAYOUT_MONO, 0);
        av_opt_set_int(swr_ctx, "out_sample_rate", 16000, 0);
        av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);

        // initialize the resampling context
        if (auto ret = swr_init(swr_ctx); ret < 0) {
            Plog::log(Plog::LogPriority::ERR, TAG, "Could not initialize resampler context");
        } else {
            Plog::log(Plog::LogPriority::INFO, TAG, "Initialized resampler context successfully");
        }

        // compute the number of converted samples: buffering is avoided
        // ensuring that the output buffer will contain at least all the
        // converted input samples
        auto dst_nb_samples = max_dst_nb_samples = av_rescale_rnd(iFrame->nb_samples, 16000, iFrame->sample_rate, AV_ROUND_UP);

        if (auto ret = av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, 1,
                static_cast<int>(dst_nb_samples), AV_SAMPLE_FMT_S16, 0);
            ret < 0) {
            Plog::log(Plog::LogPriority::ERR, TAG, "Could not allocate destination samples");
        } else {
            Plog::log(Plog::LogPriority::INFO, TAG,
                "Allocated destination samples successfully, dst_nb_samples = " + std::to_string(dst_nb_samples)
                    + " dst_linesize = " + std::to_string(dst_linesize));
        }
    }

    // compute destination number of samples
    auto dst_nb_samples = av_rescale_rnd(
        swr_get_delay(swr_ctx, iFrame->sample_rate) + iFrame->nb_samples,
        16000, iFrame->sample_rate, AV_ROUND_UP);

    // allocate more storage if not enough
    if (dst_nb_samples > max_dst_nb_samples) {
        av_free(dst_data[0]);
        if (auto ret = av_samples_alloc(dst_data, &dst_linesize, 1,
                static_cast<int>(dst_nb_samples), AV_SAMPLE_FMT_S16, 1);
            ret < 0) {
            Plog::log(Plog::LogPriority::ERR, TAG, "Could not allocate more destination samples");
        } else {
            Plog::log(Plog::LogPriority::INFO, TAG, "Allocated more destination samples successfully, dst_nb_samples = " + std::to_string(dst_nb_samples));
        }
        max_dst_nb_samples = dst_nb_samples;
    }

    // convert to destination format
    auto n_converted_samples = swr_convert(swr_ctx, dst_data,
        static_cast<int>(dst_nb_samples),
        const_cast<const std::uint8_t**>(iFrame->data), iFrame->nb_samples);
    if (n_converted_samples < 0) {
        Plog::log(Plog::LogPriority::ERR, TAG, "Error while converting");
    }

    // feed input to processing thread
    {
        std::lock_guard guard(inputLock);

        // push resulting samples to buffer
        // TODO: use a ring buffer
        auto offset = input_buffer.size();
        input_buffer.resize(offset + n_converted_samples);
        std::memcpy(&input_buffer[offset], dst_data[0], n_converted_samples * sizeof(std::int16_t));

        newFrame = true;
    }
    inputCv.notify_all();
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
