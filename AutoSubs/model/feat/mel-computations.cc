// feat/mel-computations.cc

// Copyright 2009-2011  Phonexia s.r.o.;  Karel Vesely;  Microsoft Corporation

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

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "feature-fbank.h"
#include "mel-computations.h"
#include "utils.h"

namespace kaldi {

MelBanks::MelBanks(const MelBanksOptions& opts,
    const FrameExtractionOptions& frame_opts,
    float vtln_warp_factor)
    : htk_mode_(opts.htk_mode)
{
    int num_bins = opts.num_bins;
    if (num_bins < 3)
        std::cerr << "Must have at least 3 mel bins";
    float sample_freq = frame_opts.samp_freq;
    int window_length_padded = frame_opts.PaddedWindowSize();
    assert(window_length_padded % 2 == 0);
    int num_fft_bins = window_length_padded / 2;
    float nyquist = 0.5 * sample_freq;

    float low_freq = opts.low_freq, high_freq;
    if (opts.high_freq > 0.0)
        high_freq = opts.high_freq;
    else
        high_freq = nyquist + opts.high_freq;

    if (low_freq < 0.0 || low_freq >= nyquist
        || high_freq <= 0.0 || high_freq > nyquist
        || high_freq <= low_freq)
        std::cerr << "Bad values in options: low-freq " << low_freq
                  << " and high-freq " << high_freq << " vs. nyquist "
                  << nyquist;

    float fft_bin_width = sample_freq / window_length_padded;
    // fft-bin width [think of it as Nyquist-freq / half-window-length]

    float mel_low_freq = MelScale(low_freq);
    float mel_high_freq = MelScale(high_freq);

    debug_ = opts.debug_mel;

    // divide by num_bins+1 in next line because of end-effects where the bins
    // spread out to the sides.
    float mel_freq_delta = (mel_high_freq - mel_low_freq) / (num_bins + 1);

    float vtln_low = opts.vtln_low,
          vtln_high = opts.vtln_high;
    if (vtln_high < 0.0) {
        vtln_high += nyquist;
    }

    if (vtln_warp_factor != 1.0 && (vtln_low < 0.0 || vtln_low <= low_freq || vtln_low >= high_freq || vtln_high <= 0.0 || vtln_high >= high_freq || vtln_high <= vtln_low))
        std::cerr << "Bad values in options: vtln-low " << vtln_low
                  << " and vtln-high " << vtln_high << ", versus "
                  << "low-freq " << low_freq << " and high-freq "
                  << high_freq;

    bins_.resize(num_bins);
    center_freqs_.resize(num_bins);

    for (int bin = 0; bin < num_bins; bin++) {
        float left_mel = mel_low_freq + bin * mel_freq_delta,
              center_mel = mel_low_freq + (bin + 1) * mel_freq_delta,
              right_mel = mel_low_freq + (bin + 2) * mel_freq_delta;

        if (vtln_warp_factor != 1.0) {
            left_mel = VtlnWarpMelFreq(vtln_low, vtln_high, low_freq, high_freq,
                vtln_warp_factor, left_mel);
            center_mel = VtlnWarpMelFreq(vtln_low, vtln_high, low_freq, high_freq,
                vtln_warp_factor, center_mel);
            right_mel = VtlnWarpMelFreq(vtln_low, vtln_high, low_freq, high_freq,
                vtln_warp_factor, right_mel);
        }
        center_freqs_[bin] = InverseMelScale(center_mel);
        // this_bin will be a vector of coefficients that is only
        // nonzero where this mel bin is active.
        std::vector<float> this_bin(num_fft_bins);
        int first_index = -1, last_index = -1;
        for (int i = 0; i < num_fft_bins; i++) {
            float freq = (fft_bin_width * i); // Center frequency of this fft
                // bin.
            float mel = MelScale(freq);
            if (mel > left_mel && mel < right_mel) {
                float weight;
                if (mel <= center_mel)
                    weight = (mel - left_mel) / (center_mel - left_mel);
                else
                    weight = (right_mel - mel) / (right_mel - center_mel);
                this_bin[i] = weight;
                if (first_index == -1)
                    first_index = i;
                last_index = i;
            }
        }
        assert(first_index != -1 && last_index >= first_index
            && "You may have set --num-mel-bins too large.");

        bins_[bin].first = first_index;
        int size = last_index + 1 - first_index;
        bins_[bin].second.resize(size);
        std::copy_n(this_bin.data() + first_index, size, bins_[bin].second.data());

        // Replicate a bug in HTK, for testing purposes.
        if (opts.htk_mode && bin == 0 && mel_low_freq != 0.0)
            bins_[bin].second[0] = 0.0;
    }
}

MelBanks::MelBanks(const MelBanks& other)
    : center_freqs_(other.center_freqs_)
    , bins_(other.bins_)
    , debug_(other.debug_)
    , htk_mode_(other.htk_mode_)
{
}

float MelBanks::VtlnWarpFreq(float vtln_low_cutoff, // upper+lower frequency cutoffs for VTLN.
    float vtln_high_cutoff,
    float low_freq, // upper+lower frequency cutoffs in mel computation
    float high_freq,
    float vtln_warp_factor,
    float freq)
{
    /// This computes a VTLN warping function that is not the same as HTK's one,
    /// but has similar inputs (this function has the advantage of never producing
    /// empty bins).

    /// This function computes a warp function F(freq), defined between low_freq and
    /// high_freq inclusive, with the following properties:
    ///  F(low_freq) == low_freq
    ///  F(high_freq) == high_freq
    /// The function is continuous and piecewise linear with two inflection
    ///   points.
    /// The lower inflection point (measured in terms of the unwarped
    ///  frequency) is at frequency l, determined as described below.
    /// The higher inflection point is at a frequency h, determined as
    ///   described below.
    /// If l <= f <= h, then F(f) = f/vtln_warp_factor.
    /// If the higher inflection point (measured in terms of the unwarped
    ///   frequency) is at h, then max(h, F(h)) == vtln_high_cutoff.
    ///   Since (by the last point) F(h) == h/vtln_warp_factor, then
    ///   max(h, h/vtln_warp_factor) == vtln_high_cutoff, so
    ///   h = vtln_high_cutoff / max(1, 1/vtln_warp_factor).
    ///     = vtln_high_cutoff * min(1, vtln_warp_factor).
    /// If the lower inflection point (measured in terms of the unwarped
    ///   frequency) is at l, then min(l, F(l)) == vtln_low_cutoff
    ///   This implies that l = vtln_low_cutoff / min(1, 1/vtln_warp_factor)
    ///                       = vtln_low_cutoff * max(1, vtln_warp_factor)

    if (freq < low_freq || freq > high_freq)
        return freq; // in case this gets called
    // for out-of-range frequencies, just return the freq.

    assert(vtln_low_cutoff > low_freq && "be sure to set the --vtln-low option higher than --low-freq");
    assert(vtln_high_cutoff < high_freq && "be sure to set the --vtln-high option lower than --high-freq [or negative]");
    float one = 1.0;
    float l = vtln_low_cutoff * std::max(one, vtln_warp_factor);
    float h = vtln_high_cutoff * std::min(one, vtln_warp_factor);
    float scale = 1.0 / vtln_warp_factor;
    float Fl = scale * l; // F(l);
    float Fh = scale * h; // F(h);
    assert(l > low_freq && h < high_freq);
    // slope of left part of the 3-piece linear function
    float scale_left = (Fl - low_freq) / (l - low_freq);
    // [slope of center part is just "scale"]

    // slope of right part of the 3-piece linear function
    float scale_right = (high_freq - Fh) / (high_freq - h);

    if (freq < l) {
        return low_freq + scale_left * (freq - low_freq);
    } else if (freq < h) {
        return scale * freq;
    } else { // freq >= h
        return high_freq + scale_right * (freq - high_freq);
    }
}

float MelBanks::VtlnWarpMelFreq(float vtln_low_cutoff, // upper+lower frequency cutoffs for VTLN.
    float vtln_high_cutoff,
    float low_freq, // upper+lower frequency cutoffs in mel computation
    float high_freq,
    float vtln_warp_factor,
    float mel_freq)
{
    return MelScale(VtlnWarpFreq(vtln_low_cutoff, vtln_high_cutoff,
        low_freq, high_freq,
        vtln_warp_factor, InverseMelScale(mel_freq)));
}

// "power_spectrum" contains fft energies.
void MelBanks::Compute(float const* power_spectrum,
    float* mel_energies_out) const
{
    int num_bins = bins_.size();

    for (int i = 0; i < num_bins; i++) {
        int offset = bins_[i].first;
        const std::vector<float>& v(bins_[i].second);
        float energy = VecVec(v.data(), power_spectrum + offset, v.size());
        // HTK-like flooring- for testing purposes (we prefer dither)
        if (htk_mode_ && energy < 1.0)
            energy = 1.0;
        mel_energies_out[i] = energy;

        // The following assert was added due to a problem with OpenBlas that
        // we had at one point (it was a bug in that library).  Just to detect
        // it early.
        //KALDI_ASSERT(!KALDI_ISNAN((*mel_energies_out)(i)));
    }

    if (debug_) {
        fprintf(stderr, "MEL BANKS:\n");
        for (int i = 0; i < num_bins; i++)
            fprintf(stderr, " %f", mel_energies_out[i]);
        fprintf(stderr, "\n");
    }
}

void ComputeLifterCoeffs(float Q, std::vector<float>* coeffs)
{
    // Compute liftering coefficients (scaling on cepstral coeffs)
    // coeffs are numbered slightly differently from HTK: the zeroth
    // index is C0, which is not affected.
    for (int i = 0; i < coeffs->size(); i++)
        (*coeffs)[i] = 1.0 + 0.5 * Q * sin(M_PI * i / Q);
}

// Durbin's recursion - converts autocorrelation coefficients to the LPC
// pTmp - temporal place [n]
// pAC - autocorrelation coefficients [n + 1]
// pLP - linear prediction coefficients [n] (predicted_sn = sum_1^P{a[i-1] * s[n-i]}})
//       F(z) = 1 / (1 - A(z)), 1 is not stored in the demoninator
float Durbin(int n, const float* pAC, float* pLP, float* pTmp)
{
    float ki; // reflection coefficient
    int i;
    int j;

    float E = pAC[0];

    for (i = 0; i < n; i++) {
        // next reflection coefficient
        ki = pAC[i + 1];
        for (j = 0; j < i; j++)
            ki += pLP[j] * pAC[i - j];
        ki = ki / E;

        // new error
        float c = 1 - ki * ki;
        if (c < 1.0e-5) // remove NaNs for constan signal
            c = 1.0e-5;
        E *= c;

        // new LP coefficients
        pTmp[i] = -ki;
        for (j = 0; j < i; j++)
            pTmp[j] = pLP[j] - ki * pLP[i - j - 1];

        for (j = 0; j <= i; j++)
            pLP[j] = pTmp[j];
    }

    return E;
}

void Lpc2Cepstrum(int n, const float* pLPC, float* pCepst)
{
    for (int i = 0; i < n; i++) {
        double sum = 0.0;
        int j;
        for (j = 0; j < i; j++) {
            sum += static_cast<float>(i - j) * pLPC[j] * pCepst[i - j - 1];
        }
        pCepst[i] = -pLPC[i] - sum / static_cast<float>(i + 1);
    }
}

void GetEqualLoudnessVector(const MelBanks& mel_banks,
    std::vector<float>* ans)
{
    int n = mel_banks.NumBins();
    // Central frequency of each mel bin.
    const std::vector<float>& f0 = mel_banks.GetCenterFreqs();
    ans->resize(n);
    for (int i = 0; i < n; i++) {
        float fsq = f0[i] * f0[i];
        float fsub = fsq / (fsq + 1.6e5);
        (*ans)[i] = fsub * fsub * ((fsq + 1.44e6) / (fsq + 9.61e6));
    }
}

// Compute LP coefficients from autocorrelation coefficients.
float ComputeLpc(const std::vector<float>& autocorr_in,
    std::vector<float>* lpc_out)
{
    int n = autocorr_in.size() - 1;
    assert(lpc_out->size() == n);
    std::vector<float> tmp(n);
    float ans = Durbin(n, autocorr_in.data(),
        lpc_out->data(),
        tmp.data());
    if (ans <= 0.0)
        std::clog << "Zero energy in LPC computation";
    return -std::log(1.0 / ans); // forms the C0 value
}

} // namespace kaldi
