// feat/feature-window.cc

// Copyright 2009-2011  Karel Vesely;  Petr Motlicek;  Microsoft Corporation
//           2013-2016  Johns Hopkins University (author: Daniel Povey)
//                2014  IMSL, PKU-HKUST (author: Wei Shi)

// See ../../COPYING for clarification regarding multiple authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
// WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
// MERCHANTABLITY OR NON-INFRINGEMENT.
// See the Apache 2 License for the specific language governing permissions and
// limitations under the License.

#include "feature-window.h"
#include "utils.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <numeric>

#define M_2PI (2 * M_PI)

namespace kaldi {

// State for thread-safe random number generator
struct RandomState {
    RandomState();
    unsigned seed;
};

// Returns a random integer between 0 and RAND_MAX, inclusive
int Rand(struct RandomState* state = NULL)
{
#if !defined(_POSIX_THREAD_SAFE_FUNCTIONS)
    // On Windows and Cygwin, just call Rand()
    return rand();
#else
    if (state) {
        return rand_r(&(state->seed));
    } else {
        std::lock_guard<std::mutex> lock(_RandMutex);
        return rand();
    }
#endif
}

RandomState::RandomState() {
  // we initialize it as Rand() + 27437 instead of just Rand(), because on some
  // systems, e.g. at the very least Mac OSX Yosemite and later, it seems to be
  // the case that rand_r when initialized with rand() will give you the exact
  // same sequence of numbers that rand() will give if you keep calling rand()
  // after that initial call.  This can cause problems with repeated sequences.
  // For example if you initialize two RandomState structs one after the other
  // without calling rand() in between, they would give you the same sequence
  // offset by one (if we didn't have the "+ 27437" in the code).  27437 is just
  // a randomly chosen prime number.
  seed = Rand() + 27437;
}

/// Returns a random number strictly between 0 and 1.
inline float RandUniform(struct RandomState* state = NULL)
{
    return static_cast<float>((Rand(state) + 1.0) / (RAND_MAX + 2.0));
}

inline float RandGauss(struct RandomState* state = NULL)
{
    return static_cast<float>(sqrtf(-2 * std::log(RandUniform(state)))
        * cosf(2 * M_PI * RandUniform(state)));
}

void Dither(float* waveform, int size, float dither_value)
{
    if (dither_value == 0.0)
        return;
    float* data = waveform;
    RandomState rstate;
    for (int i = 0; i < size; i++)
        data[i] += RandGauss(&rstate) * dither_value;
}

void Preemphasize(float* waveform, int size, float preemph_coeff)
{
    if (preemph_coeff == 0.0)
        return;
    assert(preemph_coeff >= 0.0 && preemph_coeff <= 1.0);
    for (int i = size - 1; i > 0; i--)
        waveform[i] -= preemph_coeff * waveform[i - 1];
    waveform[0] -= preemph_coeff * waveform[0];
}

std::int64_t FirstSampleOfFrame(int frame,
    const FrameExtractionOptions& opts)
{
    std::int64_t frame_shift = opts.WindowShift();
    if (opts.snip_edges) {
        return frame * frame_shift;
    } else {
        std::int64_t midpoint_of_frame = frame_shift * frame + frame_shift / 2,
                     beginning_of_frame = midpoint_of_frame - opts.WindowSize() / 2;
        return beginning_of_frame;
    }
}

int NumFrames(std::int64_t num_samples,
    const FrameExtractionOptions& opts,
    bool flush)
{
    std::int64_t frame_shift = opts.WindowShift();
    std::int64_t frame_length = opts.WindowSize();
    if (opts.snip_edges) {
        // with --snip-edges=true (the default), we use a HTK-like approach to
        // determining the number of frames-- all frames have to fit completely into
        // the waveform, and the first frame begins at sample zero.
        if (num_samples < frame_length)
            return 0;
        else
            return (1 + ((num_samples - frame_length) / frame_shift));
        // You can understand the expression above as follows: 'num_samples -
        // frame_length' is how much room we have to shift the frame within the
        // waveform; 'frame_shift' is how much we shift it each time; and the ratio
        // is how many times we can shift it (integer arithmetic rounds down).
    } else {
        // if --snip-edges=false, the number of frames is determined by rounding the
        // (file-length / frame-shift) to the nearest integer.  The point of this
        // formula is to make the number of frames an obvious and predictable
        // function of the frame shift and signal length, which makes many
        // segmentation-related questions simpler.
        //
        // Because integer division in C++ rounds toward zero, we add (half the
        // frame-shift minus epsilon) before dividing, to have the effect of
        // rounding towards the closest integer.
        int num_frames = (num_samples + (frame_shift / 2)) / frame_shift;

        if (flush)
            return num_frames;

        // note: 'end' always means the last plus one, i.e. one past the last.
        std::int64_t end_sample_of_last_frame = FirstSampleOfFrame(num_frames - 1, opts)
            + frame_length;

        // the following code is optimized more for clarity than efficiency.
        // If flush == false, we can't output frames that extend past the end
        // of the signal.
        while (num_frames > 0 && end_sample_of_last_frame > num_samples) {
            num_frames--;
            end_sample_of_last_frame -= frame_shift;
        }
        return num_frames;
    }
}

FeatureWindowFunction::FeatureWindowFunction(const FrameExtractionOptions& opts)
{
    int frame_length = opts.WindowSize();
    assert(frame_length > 0);
    window.resize(frame_length);
    double a = M_2PI / (frame_length - 1);
    for (int i = 0; i < frame_length; i++) {
        double i_fl = static_cast<double>(i);
        if (opts.window_type == "hanning") {
            window[i] = 0.5 - 0.5 * cos(a * i_fl);
        } else if (opts.window_type == "sine") {
            // when you are checking ws wikipedia, please
            // note that 0.5 * a = M_PI/(frame_length-1)
            window[i] = sin(0.5 * a * i_fl);
        } else if (opts.window_type == "hamming") {
            window[i] = 0.54 - 0.46 * cos(a * i_fl);
        } else if (opts.window_type == "povey") { // like hamming but goes to zero at edges.
            window[i] = pow(0.5 - 0.5 * cos(a * i_fl), 0.85);
        } else if (opts.window_type == "rectangular") {
            window[i] = 1.0;
        } else if (opts.window_type == "blackman") {
            window[i] = opts.blackman_coeff - 0.5 * cos(a * i_fl) + (0.5 - opts.blackman_coeff) * cos(2 * a * i_fl);
        } else {
            std::cerr << "Invalid window type " << opts.window_type;
        }
    }
}

void ProcessWindow(const FrameExtractionOptions& opts,
    const FeatureWindowFunction& window_function,
    float* window, std::size_t window_size,
    float* log_energy_pre_window)
{
    int frame_length = opts.WindowSize();
    assert(window_size == frame_length);

    if (opts.dither != 0.0)
        Dither(window, window_size, opts.dither);

    if (opts.remove_dc_offset) {
        float sum = std::accumulate(window, window + window_size, 0.f);
        for (std::size_t i = 0; i < window_size; ++i) {
            window[i] += -sum / frame_length;
        }
    }

    if (log_energy_pre_window != NULL) {
        float energy = std::max<float>(VecVec(window, window, window_size),
            std::numeric_limits<float>::epsilon());
        *log_energy_pre_window = std::log(energy);
    }

    if (opts.preemph_coeff != 0.0)
        Preemphasize(window, window_size, opts.preemph_coeff);

    for (std::size_t i = 0; i < window_size; ++i) {
        window[i] *= window_function.window[i];
    }
}

// ExtractWindow extracts a windowed frame of waveform with a power-of-two,
// padded size.  It does mean subtraction, pre-emphasis and dithering as
// requested.
void ExtractWindow(std::int64_t sample_offset,
    const std::vector<float>& wave,
    int f, // with 0 <= f < NumFrames(feats, opts)
    const FrameExtractionOptions& opts,
    const FeatureWindowFunction& window_function,
    std::vector<float>* window,
    float* log_energy_pre_window)
{
    assert(sample_offset >= 0 && wave.size() != 0);
    int frame_length = opts.WindowSize(),
        frame_length_padded = opts.PaddedWindowSize();
    std::int64_t num_samples = sample_offset + wave.size(),
                 start_sample = FirstSampleOfFrame(f, opts),
                 end_sample = start_sample + frame_length;

    if (opts.snip_edges) {
        assert(start_sample >= sample_offset && end_sample <= num_samples);
    } else {
        assert(sample_offset == 0 || start_sample >= sample_offset);
    }

    if (window->size() != frame_length_padded)
        window->resize(frame_length_padded, 1);

    // wave_start and wave_end are start and end indexes into 'wave', for the
    // piece of wave that we're trying to extract.
    int wave_start = int(start_sample - sample_offset),
        wave_end = wave_start + frame_length;
    if (wave_start >= 0 && wave_end <= wave.size()) {
        // the normal case-- no edge effects to consider.
        std::copy_n(wave.data() + wave_start, frame_length, window->data());
    } else {
        // Deal with any end effects by reflection, if needed.  This code will only
        // be reached for about two frames per utterance, so we don't concern
        // ourselves excessively with efficiency.
        int wave_dim = wave.size();
        for (int s = 0; s < frame_length; s++) {
            int s_in_wave = s + wave_start;
            while (s_in_wave < 0 || s_in_wave >= wave_dim) {
                // reflect around the beginning or end of the wave.
                // e.g. -1 -> 0, -2 -> 1.
                // dim -> dim - 1, dim + 1 -> dim - 2.
                // the code supports repeated reflections, although this
                // would only be needed in pathological cases.
                if (s_in_wave < 0)
                    s_in_wave = -s_in_wave - 1;
                else
                    s_in_wave = 2 * wave_dim - 1 - s_in_wave;
            }
            (*window)[s] = wave[s_in_wave];
        }
    }

    if (frame_length_padded > frame_length) {
        std::fill_n(window->data() + frame_length, frame_length_padded - frame_length, 0.f);
    }

    ProcessWindow(opts, window_function, window->data(), frame_length, log_energy_pre_window);
}

} // namespace kaldi
