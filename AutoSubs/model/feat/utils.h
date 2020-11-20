#ifndef UTILS_H
#define UTILS_H

#include <vector>

namespace kaldi {

template <typename T>
inline float VecVec(T const* a, T const* b, int size)
{
    T sum{};
    for (int i = 0; i < size; ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

/// RealFft is a fourier transform of real inputs.  Internally it uses
/// ComplexFft.  The input dimension N must be even.  If forward == true,
/// it transforms from a sequence of N real points to its complex fourier
/// transform; otherwise it goes in the reverse direction.  If you call it
/// in the forward and then reverse direction and multiply by 1.0/N, you
/// will get back the original data.
/// The interpretation of the complex-FFT data is as follows: the array
/// is a sequence of complex numbers C_n of length N/2 with (real, im) format,
/// i.e. [real0, real_{N/2}, real1, im1, real2, im2, real3, im3, ...].
/// See also SplitRadixRealFft, declared in srfft.h, which is more efficient
/// but only works if the length of the input is a power of 2.
template <typename Real>
void RealFft(std::vector<Real>* v, bool forward);
}

#endif // UTILS_H
