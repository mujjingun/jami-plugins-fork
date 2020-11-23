#ifndef ONLINEFEATURE_H
#define ONLINEFEATURE_H

#include "feature-fbank.h"

namespace kaldi {

/// This class serves as a storage for feature vectors with an option to limit
/// the memory usage by removing old elements. The deleted frames indices are
/// "remembered" so that regardless of the MAX_ITEMS setting, the user always
/// provides the indices as if no deletion was being performed.
/// This is useful when processing very long recordings which would otherwise
/// cause the memory to eventually blow up when the features are not being removed.
class RecyclingVector {
public:
    /// By default it does not remove any elements.
    RecyclingVector(int items_to_hold = -1);

    /// The ownership is being retained by this collection - do not delete the item.
    std::vector<float>* At(int index) const;

    /// The ownership of the item is passed to this collection - do not delete the item.
    void PushBack(std::vector<float>* item);

    /// This method returns the size as if no "recycling" had happened,
    /// i.e. equivalent to the number of times the PushBack method has been called.
    int Size() const;

    ~RecyclingVector();

private:
    std::deque<std::vector<float>*> items_;
    int items_to_hold_;
    int first_available_index_;
};

/**
   OnlineFeatureInterface is an interface for online feature processing (it is
   also usable in the offline setting, but currently we're not using it for
   that).  This is for use in the online2/ directory, and it supersedes the
   interface in ../online/online-feat-input.h.  We have a slightly different
   model that puts more control in the hands of the calling thread, and won't
   involve waiting on semaphores in the decoding thread.

   This interface only specifies how the object *outputs* the features.
   How it obtains the features, e.g. from a previous object or objects of type
   OnlineFeatureInterface, is not specified in the interface and you will
   likely define new constructors or methods in the derived type to do that.

   You should appreciate that this interface is designed to allow random
   access to features, as long as they are ready.  That is, the user
   can call GetFrame for any frame less than NumFramesReady(), and when
   implementing a child class you must not make assumptions about the
   order in which the user makes these calls.
*/

class OnlineFeatureInterface {
public:
    virtual int Dim() const = 0; /// returns the feature dimension.

    /// Returns the total number of frames, since the start of the utterance, that
    /// are now available.  In an online-decoding context, this will likely
    /// increase with time as more data becomes available.
    virtual int NumFramesReady() const = 0;

    /// Returns true if this is the last frame.  Frame indices are zero-based, so the
    /// first frame is zero.  IsLastFrame(-1) will return false, unless the file
    /// is empty (which is a case that I'm not sure all the code will handle, so
    /// be careful).  This function may return false for some frame if
    /// we haven't yet decided to terminate decoding, but later true if we decide
    /// to terminate decoding.  This function exists mainly to correctly handle
    /// end effects in feature extraction, and is not a mechanism to determine how
    /// many frames are in the decodable object (as it used to be, and for backward
    /// compatibility, still is, in the Decodable interface).
    virtual bool IsLastFrame(int frame) const = 0;

    /// Gets the feature vector for this frame.  Before calling this for a given
    /// frame, it is assumed that you called NumFramesReady() and it returned a
    /// number greater than "frame".  Otherwise this call will likely crash with
    /// an assert failure.  This function is not declared const, in case there is
    /// some kind of caching going on, but most of the time it shouldn't modify
    /// the class.
    virtual void GetFrame(int frame, std::vector<float>* feat) = 0;

    // Returns frame shift in seconds.  Helps to estimate duration from frame
    // counts.
    virtual float FrameShiftInSeconds() const = 0;

    /// Virtual destructor.  Note: constructors that take another member of
    /// type OnlineFeatureInterface are not expected to take ownership of
    /// that pointer; the caller needs to keep track of that manually.
    virtual ~OnlineFeatureInterface() {}
};

/// Add a virtual class for "source" features such as MFCC or PLP or pitch
/// features.
class OnlineBaseFeature : public OnlineFeatureInterface {
public:
    /// This would be called from the application, when you get more wave data.
    /// Note: the sampling_rate is typically only provided so the code can assert
    /// that it matches the sampling rate expected in the options.
    virtual void AcceptWaveform(float sampling_rate,
        const std::vector<float>& waveform)
        = 0;

    /// InputFinished() tells the class you won't be providing any
    /// more waveform.  This will help flush out the last few frames
    /// of delta or LDA features (it will typically affect the return value
    /// of IsLastFrame.
    virtual void InputFinished() = 0;
};

/// This is a templated class for online feature extraction;
/// it's templated on a class like MfccComputer or PlpComputer
/// that does the basic feature extraction.
template <class C>
class OnlineGenericBaseFeature : public OnlineBaseFeature {
public:
    //
    // First, functions that are present in the interface:
    //
    virtual int Dim() const { return computer_.Dim(); }

    // Note: IsLastFrame() will only ever return true if you have called
    // InputFinished() (and this frame is the last frame).
    virtual bool IsLastFrame(int frame) const
    {
        return input_finished_ && frame == NumFramesReady() - 1;
    }
    virtual float FrameShiftInSeconds() const
    {
        return computer_.GetFrameOptions().frame_shift_ms / 1000.0f;
    }

    virtual int NumFramesReady() const { return features_.Size(); }

    virtual void GetFrame(int frame, std::vector<float>* feat);

    // Next, functions that are not in the interface.

    // Constructor from options class
    explicit OnlineGenericBaseFeature(const typename C::Options& opts);

    // This would be called from the application, when you get
    // more wave data.  Note: the sampling_rate is only provided so
    // the code can assert that it matches the sampling rate
    // expected in the options.
    virtual void AcceptWaveform(float sampling_rate,
        const std::vector<float>& waveform);

    // InputFinished() tells the class you won't be providing any
    // more waveform.  This will help flush out the last frame or two
    // of features, in the case where snip-edges == false; it also
    // affects the return value of IsLastFrame().
    virtual void InputFinished();

private:
    // This function computes any additional feature frames that it is possible to
    // compute from 'waveform_remainder_', which at this point may contain more
    // than just a remainder-sized quantity (because AcceptWaveform() appends to
    // waveform_remainder_ before calling this function).  It adds these feature
    // frames to features_, and shifts off any now-unneeded samples of input from
    // waveform_remainder_ while incrementing waveform_offset_ by the same amount.
    void ComputeFeatures();

    void MaybeCreateResampler(float sampling_rate);

    C computer_; // class that does the MFCC or PLP or filterbank computation

    FeatureWindowFunction window_function_;

    // features_ is the Mfcc or Plp or Fbank features that we have already computed.

    RecyclingVector features_;

    // True if the user has called "InputFinished()"
    bool input_finished_;

    // The sampling frequency, extracted from the config.  Should
    // be identical to the waveform supplied.
    float sampling_frequency_;

    // waveform_offset_ is the number of samples of waveform that we have
    // already discarded, i.e. that were prior to 'waveform_remainder_'.
    std::int64_t waveform_offset_;

    // waveform_remainder_ is a short piece of waveform that we may need to keep
    // after extracting all the whole frames we can (whatever length of feature
    // will be required for the next phase of computation).
    std::vector<float> waveform_remainder_;
};

typedef OnlineGenericBaseFeature<FbankComputer> OnlineFbank;
}

#endif // ONLINEFEATURE_H
