// feat/feature-fbank.cc

// Copyright 2009-2012  Karel Vesely
//                2016  Johns Hopkins University (author: Daniel Povey)

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

#include "feature-fbank.h"

#include "utils.h"
#include <algorithm>

namespace kaldi {

FbankComputer::FbankComputer(const FbankOptions& opts)
    : opts_(opts)
    , srfft_(NULL)
{
    if (opts.energy_floor > 0.0)
        log_energy_floor_ = std::log(opts.energy_floor);

    int padded_window_size = opts.frame_opts.PaddedWindowSize();
    if ((padded_window_size & (padded_window_size - 1)) == 0) // Is a power of two...
        srfft_ = new SplitRadixRealFft<float>(padded_window_size);

    // We'll definitely need the filterbanks info for VTLN warping factor 1.0.
    // [note: this call caches it.]
    GetMelBanks(1.0);
}

FbankComputer::FbankComputer(const FbankComputer& other)
    : opts_(other.opts_)
    , log_energy_floor_(other.log_energy_floor_)
    , mel_banks_(other.mel_banks_)
    , srfft_(NULL)
{
    for (std::map<float, MelBanks*>::iterator iter = mel_banks_.begin();
         iter != mel_banks_.end();
         ++iter)
        iter->second = new MelBanks(*(iter->second));
    if (other.srfft_)
        srfft_ = new SplitRadixRealFft<float>(*(other.srfft_));
}

FbankComputer::~FbankComputer()
{
    for (std::map<float, MelBanks*>::iterator iter = mel_banks_.begin();
         iter != mel_banks_.end(); ++iter)
        delete iter->second;
    delete srfft_;
}

const MelBanks* FbankComputer::GetMelBanks(float vtln_warp)
{
    MelBanks* this_mel_banks = NULL;
    std::map<float, MelBanks*>::iterator iter = mel_banks_.find(vtln_warp);
    if (iter == mel_banks_.end()) {
        this_mel_banks = new MelBanks(opts_.mel_opts,
            opts_.frame_opts,
            vtln_warp);
        mel_banks_[vtln_warp] = this_mel_banks;
    } else {
        this_mel_banks = iter->second;
    }
    return this_mel_banks;
}

// ComputePowerSpectrum converts a complex FFT (as produced by the FFT
// functions in matrix/matrix-functions.h), and converts it into
// a power spectrum.  If the complex FFT is a vector of size n (representing
// half the complex FFT of a real signal of size n, as described there),
// this function computes in the first (n/2) + 1 elements of it, the
// energies of the fft bins from zero to the Nyquist frequency.  Contents of the
// remaining (n/2) - 1 elements are undefined at output.
void ComputePowerSpectrum(std::vector<float>* waveform)
{
    int dim = waveform->size();

    // no, letting it be non-power-of-two for now.
    // KALDI_ASSERT(dim > 0 && (dim & (dim-1) == 0));  // make sure a power of two.. actually my FFT code
    // does not require this (dan) but this is better in case we use different code [dan].

    // RealFft(waveform, true);  // true == forward (not inverse) FFT; makes no difference here,
    // as we just want power spectrum.

    // now we have in waveform, first half of complex spectrum
    // it's stored as [real0, realN/2, real1, im1, real2, im2, ...]
    int half_dim = dim / 2;
    float first_energy = (*waveform)[0] * (*waveform)[0],
          last_energy = (*waveform)[1] * (*waveform)[1]; // handle this special case
    for (int i = 1; i < half_dim; i++) {
        float real = (*waveform)[i * 2], im = (*waveform)[i * 2 + 1];
        (*waveform)[i] = real * real + im * im;
    }
    (*waveform)[0] = first_energy;
    (*waveform)[half_dim] = last_energy; // Will actually never be used, and anyway
    // if the signal has been bandlimited sensibly this should be zero.
}

void FbankComputer::Compute(float signal_raw_log_energy,
    float vtln_warp,
    std::vector<float>* signal_frame,
    std::vector<float>* feature)
{
    const MelBanks& mel_banks = *(GetMelBanks(vtln_warp));

    assert(signal_frame->size() == opts_.frame_opts.PaddedWindowSize() && feature->size() == this->Dim());

    // Compute energy after window function (not the raw one).
    if (opts_.use_energy && !opts_.raw_energy)
        signal_raw_log_energy = std::log(std::max<float>(VecVec(signal_frame->data(), signal_frame->data(), signal_frame->size()),
            std::numeric_limits<float>::epsilon()));

    if (srfft_ != NULL) // Compute FFT using split-radix algorithm.
        srfft_->Compute(signal_frame->data(), true);
    else // An alternative algorithm that works for non-powers-of-two.
        RealFft(signal_frame, true);

    // Convert the FFT into a power spectrum.
    ComputePowerSpectrum(signal_frame);
    float* power_spectrum = signal_frame->data();
    int power_spectrum_size = signal_frame->size() / 2 + 1;

    // Use magnitude instead of power if requested.
    if (!opts_.use_power) {
        for (int i = 0; i < power_spectrum_size; ++i) {
            power_spectrum[i] = std::pow(power_spectrum[i], 0.5);
        }
    }

    int mel_offset = ((opts_.use_energy && !opts_.htk_compat) ? 1 : 0);
    float* mel_energies = feature->data() + mel_offset;
    int mel_energies_size = opts_.mel_opts.num_bins;

    // Sum with mel fiterbanks over the power spectrum
    mel_banks.Compute(power_spectrum, mel_energies);
    if (opts_.use_log_fbank) {
        // Avoid log of zero (which should be prevented anyway by dithering).
        for (int i = 0; i < mel_energies_size; ++i) {
            if (mel_energies[i] < std::numeric_limits<float>::epsilon()) {
                mel_energies[i] = std::numeric_limits<float>::epsilon();
            }
            mel_energies[i] = std::log(mel_energies[i]);
        }
    }

    // Copy energy as first value (or the last, if htk_compat == true).
    if (opts_.use_energy) {
        if (opts_.energy_floor > 0.0 && signal_raw_log_energy < log_energy_floor_) {
            signal_raw_log_energy = log_energy_floor_;
        }
        int energy_index = opts_.htk_compat ? opts_.mel_opts.num_bins : 0;
        (*feature)[energy_index] = signal_raw_log_energy;
    }
}

} // namespace kaldi
