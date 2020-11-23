#include "online-feature.h"

#include <iostream>
#include <algorithm>

namespace kaldi {
RecyclingVector::RecyclingVector(int items_to_hold)
    : items_to_hold_(items_to_hold == 0 ? -1 : items_to_hold)
    , first_available_index_(0)
{
}

RecyclingVector::~RecyclingVector()
{
    for (auto* item : items_) {
        delete item;
    }
}

std::vector<float>* RecyclingVector::At(int index) const
{
    if (index < first_available_index_) {
        std::cerr << "Attempted to retrieve feature vector that was "
                     "already removed by the RecyclingVector (index = "
                  << index << "; "
                  << "first_available_index = " << first_available_index_ << "; "
                  << "size = " << Size() << ")";
    }
    // 'at' does size checking.
    return items_.at(index - first_available_index_);
}

void RecyclingVector::PushBack(std::vector<float>* item)
{
    if (items_.size() == items_to_hold_) {
        delete items_.front();
        items_.pop_front();
        ++first_available_index_;
    }
    items_.push_back(item);
}

int RecyclingVector::Size() const
{
    return first_available_index_ + items_.size();
}

template <class C>
void OnlineGenericBaseFeature<C>::GetFrame(int frame,
    std::vector<float>* feat)
{
    auto from = features_.At(frame);
    std::copy(from->begin(), from->end(), feat->begin());
};

template <class C>
OnlineGenericBaseFeature<C>::OnlineGenericBaseFeature(
    const typename C::Options& opts)
    : computer_(opts)
    , window_function_(computer_.GetFrameOptions())
    , features_(opts.frame_opts.max_feature_vectors)
    , input_finished_(false)
    , waveform_offset_(0)
{
    // RE the following assert: search for ONLINE_IVECTOR_LIMIT in
    // online-ivector-feature.cc.
    // Casting to std::uint32_t, an unsigned type, means that -1 would be treated
    // as `very large`.
    assert(static_cast<std::uint32_t>(opts.frame_opts.max_feature_vectors) > 200);
}

template <class C>
void OnlineGenericBaseFeature<C>::MaybeCreateResampler(
    float sampling_rate)
{
    float expected_sampling_rate = computer_.GetFrameOptions().samp_freq;
    if (sampling_rate != expected_sampling_rate) {
        std::cerr << "Sampling frequency mismatch, expected "
                  << expected_sampling_rate << ", got " << sampling_rate
                  << "\nPerhaps you want to use the options "
                     "--allow_{upsample,downsample}";
    }
}

template <class C>
void OnlineGenericBaseFeature<C>::InputFinished()
{
    input_finished_ = true;
    ComputeFeatures();
}

template <class C>
void OnlineGenericBaseFeature<C>::AcceptWaveform(
    float sampling_rate, const std::vector<float>& original_waveform)
{
    if (original_waveform.size() == 0)
        return; // Nothing to do.
    if (input_finished_)
        std::cerr << "AcceptWaveform called after InputFinished() was called.";

    std::vector<float> appended_wave;
    std::vector<float> resampled_wave;

    const std::vector<float>* waveform;

    MaybeCreateResampler(sampling_rate);
    waveform = &original_waveform;

    appended_wave.resize(waveform_remainder_.size() + waveform->size());
    if (waveform_remainder_.size() != 0) {
        std::copy_n(waveform_remainder_.data(), waveform_remainder_.size(), appended_wave.data());
    }
    std::copy_n(waveform->data(), waveform->size(), appended_wave.data() + waveform_remainder_.size());
    waveform_remainder_.swap(appended_wave);
    ComputeFeatures();
}

template <class C>
void OnlineGenericBaseFeature<C>::ComputeFeatures()
{
    const FrameExtractionOptions& frame_opts = computer_.GetFrameOptions();
    std::int64_t num_samples_total = waveform_offset_ + waveform_remainder_.size();
    int num_frames_old = features_.Size(),
        num_frames_new = NumFrames(num_samples_total, frame_opts,
            input_finished_);
    assert(num_frames_new >= num_frames_old);

    std::vector<float> window;
    bool need_raw_log_energy = computer_.NeedRawLogEnergy();
    for (int frame = num_frames_old; frame < num_frames_new; frame++) {
        float raw_log_energy = 0.0;
        ExtractWindow(waveform_offset_, waveform_remainder_, frame,
            frame_opts, window_function_, &window,
            need_raw_log_energy ? &raw_log_energy : NULL);
        std::vector<float>* this_feature = new std::vector<float>(computer_.Dim(), 1);
        // note: this online feature-extraction code does not support VTLN.
        float vtln_warp = 1.0;
        computer_.Compute(raw_log_energy, vtln_warp, &window, this_feature);
        features_.PushBack(this_feature);
    }
    // OK, we will now discard any portion of the signal that will not be
    // necessary to compute frames in the future.
    std::int64_t first_sample_of_next_frame = FirstSampleOfFrame(num_frames_new,
        frame_opts);
    int samples_to_discard = first_sample_of_next_frame - waveform_offset_;
    if (samples_to_discard > 0) {
        // discard the leftmost part of the waveform that we no longer need.
        int new_num_samples = waveform_remainder_.size() - samples_to_discard;
        if (new_num_samples <= 0) {
            // odd, but we'll try to handle it.
            waveform_offset_ += waveform_remainder_.size();
            waveform_remainder_.resize(0);
        } else {
            std::vector<float> new_remainder(new_num_samples);
            std::copy_n(waveform_remainder_.data() + samples_to_discard, new_num_samples, new_remainder.data());
            waveform_offset_ += samples_to_discard;
            waveform_remainder_.swap(new_remainder);
        }
    }
}

template class OnlineGenericBaseFeature<FbankComputer>;
}
