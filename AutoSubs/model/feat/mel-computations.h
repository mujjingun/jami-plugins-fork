// feat/mel-computations.h

// Copyright 2009-2011  Phonexia s.r.o.;  Microsoft Corporation
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

#ifndef KALDI_FEAT_MEL_COMPUTATIONS_H_
#define KALDI_FEAT_MEL_COMPUTATIONS_H_

#include <complex>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <utility>
#include <vector>

namespace kaldi {
/// @addtogroup  feat FeatureExtraction
/// @{

struct FrameExtractionOptions; // defined in feature-window.h

struct MelBanksOptions {
    int num_bins; // e.g. 25; number of triangular bins
    float low_freq; // e.g. 20; lower frequency cutoff
    float high_freq; // an upper frequency cutoff; 0 -> no cutoff, negative
    // ->added to the Nyquist frequency to get the cutoff.
    float vtln_low; // vtln lower cutoff of warping function.
    float vtln_high; // vtln upper cutoff of warping function: if negative, added
        // to the Nyquist frequency to get the cutoff.
    bool debug_mel;
    // htk_mode is a "hidden" config, it does not show up on command line.
    // Enables more exact compatibility with HTK, for testing purposes.  Affects
    // mel-energy flooring and reproduces a bug in HTK.
    bool htk_mode;
    explicit MelBanksOptions(int num_bins = 25)
        : num_bins(num_bins)
        , low_freq(20)
        , high_freq(0)
        , vtln_low(100)
        , vtln_high(-500)
        , debug_mel(false)
        , htk_mode(false)
    {
    }
};

class MelBanks {
public:
    static inline float InverseMelScale(float mel_freq)
    {
        return 700.0f * (expf(mel_freq / 1127.0f) - 1.0f);
    }

    static inline float MelScale(float freq)
    {
        return 1127.0f * logf(1.0f + freq / 700.0f);
    }

    static float VtlnWarpFreq(float vtln_low_cutoff,
        float vtln_high_cutoff, // discontinuities in warp func
        float low_freq,
        float high_freq, // upper+lower frequency cutoffs in
        // the mel computation
        float vtln_warp_factor,
        float freq);

    static float VtlnWarpMelFreq(float vtln_low_cutoff,
        float vtln_high_cutoff,
        float low_freq,
        float high_freq,
        float vtln_warp_factor,
        float mel_freq);

    MelBanks(const MelBanksOptions& opts,
        const FrameExtractionOptions& frame_opts,
        float vtln_warp_factor);

    /// Compute Mel energies (note: not log enerties).
    /// At input, "fft_energies" contains the FFT energies (not log).
    void Compute(const float *fft_energies,
        float *mel_energies_out) const;

    int NumBins() const { return bins_.size(); }

    // returns vector of central freq of each bin; needed by plp code.
    const std::vector<float>& GetCenterFreqs() const { return center_freqs_; }

    const std::vector<std::pair<int, std::vector<float>>>& GetBins() const
    {
        return bins_;
    }

    // Copy constructor
    MelBanks(const MelBanks& other);

private:
    // Disallow assignment
    MelBanks& operator=(const MelBanks& other);

    // center frequencies of bins, numbered from 0 ... num_bins-1.
    // Needed by GetCenterFreqs().
    std::vector<float> center_freqs_;

    // the "bins_" vector is a vector, one for each bin, of a pair:
    // (the first nonzero fft-bin), (the vector of weights).
    std::vector<std::pair<int, std::vector<float>>> bins_;

    bool debug_;
    bool htk_mode_;
};

// Compute liftering coefficients (scaling on cepstral coeffs)
// coeffs are numbered slightly differently from HTK: the zeroth
// index is C0, which is not affected.
void ComputeLifterCoeffs(float Q, std::vector<float>* coeffs);

// Durbin's recursion - converts autocorrelation coefficients to the LPC
// pTmp - temporal place [n]
// pAC - autocorrelation coefficients [n + 1]
// pLP - linear prediction coefficients [n] (predicted_sn = sum_1^P{a[i-1] * s[n-i]}})
//       F(z) = 1 / (1 - A(z)), 1 is not stored in the denominator
// Returns log energy of residual (I think)
float Durbin(int n, const float* pAC, float* pLP, float* pTmp);

// Compute LP coefficients from autocorrelation coefficients.
// Returns log energy of residual (I think)
float ComputeLpc(const std::vector<float>& autocorr_in,
    std::vector<float>* lpc_out);

void Lpc2Cepstrum(int n, const float* pLPC, float* pCepst);

void GetEqualLoudnessVector(const MelBanks& mel_banks,
    std::vector<float>* ans);

/// @} End of "addtogroup feat"
} // namespace kaldi

#endif // KALDI_FEAT_MEL_COMPUTATIONS_H_
