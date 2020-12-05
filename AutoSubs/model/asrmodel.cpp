#include "asrmodel.h"

// torch
#include <torch/script.h>
#include <torch/torch.h>

// vad
#include <fvad.h>

// logging
#include <pluglog.h>

#include <algorithm>
#include <fstream>

#include "codes.h"
#include "feat/online-feature.h"
#include "utf.hpp"

static const std::string TAG = "AutoSub";

static const kaldi::FbankOptions options = [] {
    kaldi::FbankOptions options;
    options.mel_opts.num_bins = 80;
    options.frame_opts.frame_length_ms = 20.0f;
    options.frame_opts.frame_shift_ms = 10.0f;
    options.frame_opts.window_type = "hamming";
    options.frame_opts.max_feature_vectors = 1000; // 10s of audio
    return options;
}();

struct ASRModelPimpl {
    torch::Device device;
    torch::jit::script::Module module;
    std::unique_ptr<kaldi::OnlineFbank> fbank;
    Fvad* vad;
    std::ofstream log_file;
    bool first_input = true;
    std::chrono::system_clock::time_point start_time{};
};

ASRModel::ASRModel(std::string const& model_path) try
    : pimpl(new ASRModelPimpl{
          torch::Device(torch::kCPU),
          torch::jit::load(model_path),
          std::make_unique<kaldi::OnlineFbank>(options),
          fvad_new(),
          std::ofstream("asr_log.txt", std::ios::app),
      }),
      input_size(options.frame_opts.PaddedWindowSize()),
      max_num_feats(options.frame_opts.max_feature_vectors),
      vad_frame_length(16000 / 1000 * 10) // 10ms frames
{
    pimpl->module.to(pimpl->device);
    Plog::log(Plog::LogPriority::INFO, TAG, "Torch module loaded successfully");

    if (fvad_set_mode(pimpl->vad, 3) < 0) {
        Plog::log(Plog::LogPriority::ERR, TAG, "invalid vad mode");
        throw std::runtime_error("invalid vad mode");
    }

    if (fvad_set_sample_rate(pimpl->vad, 16000) < 0) {
        Plog::log(Plog::LogPriority::ERR, TAG, "invalid vad sample rate");
        throw std::runtime_error("invalid sample rate");
    }

    vad_input_buf.reserve(vad_frame_length);
    input_buf.reserve(vad_frame_length);
    feature.resize(options.mel_opts.num_bins);
    feature_buf.reserve(options.mel_opts.num_bins * max_num_feats);
} catch (const c10::Error& e) {
    Plog::log(Plog::LogPriority::ERR, TAG, "Could not load torch model");
    throw;
}

void ASRModel::reset()
{
    // reset model state
    pimpl->fbank = std::make_unique<kaldi::OnlineFbank>(options);
    pimpl->first_input = true;
    counter = 0;
    voice_activity = false;
    vad_input_buf.clear();
    input_buf.clear();
    feature_offset = 0;
    feature_buf.clear();

    // flush log file
    pimpl->log_file.flush();
}

ASRModel::~ASRModel()
{
    fvad_free(pimpl->vad);
}

std::string ASRModel::process(int16_t* buf, int size)
{
    torch::NoGradGuard no_grad;

    bool segment_ended = false;

    if (pimpl->first_input) {
        pimpl->first_input = false;
        pimpl->start_time = std::chrono::system_clock::now();
        pimpl->log_file << "=====================================================\n";
    }

    // voice activity detection
    for (int i = 0; i < size; ++i, ++counter) {
        vad_input_buf.push_back(buf[i]);

        if (vad_input_buf.size() == std::size_t(vad_frame_length)) {
            int vad_result = fvad_process(pimpl->vad, vad_input_buf.data(), vad_frame_length);

            if (vad_result < 0) {
                Plog::log(Plog::LogPriority::ERR, TAG, "VAD Processing failed");
                vad_input_buf.clear();
                return {};
            }

            if (!voice_activity && vad_result) {
                voice_activity = true;
                Plog::log(Plog::LogPriority::INFO, TAG, "voice start = " + std::to_string(counter / 16000.f));
            } else if (voice_activity && !vad_result) {
                voice_activity = false;
                segment_ended = true;
                Plog::log(Plog::LogPriority::INFO, TAG, "voice end = " + std::to_string((counter - vad_frame_length) / 16000.f));
            }

            // convert buf into fbank features
            if (voice_activity) {
                input_buf.resize(vad_frame_length);
                for (int j = 0; j < vad_frame_length; ++j) {
                    input_buf[j] = vad_input_buf[j] / 32767.f;
                }
                pimpl->fbank->AcceptWaveform(16000, input_buf);
            }

            vad_input_buf.clear();
        }
    }

    // process ASR if voice activity ended
    if (segment_ended) {
        return extract_text();
    }

    return {};
}

std::string ASRModel::extract_text()
{
    Plog::log(Plog::LogPriority::INFO, TAG, "Extracting text..");

    std::u32string str{};

    feature_buf.clear();
    feature_offset = std::max(feature_offset, pimpl->fbank->NumFramesReady() - max_num_feats);
    for (; feature_offset < pimpl->fbank->NumFramesReady(); ++feature_offset) {
        pimpl->fbank->GetFrame(feature_offset, &feature);
        std::copy(feature.begin(), feature.end(), std::back_inserter(feature_buf));
    }

    // normalize features
    float avg = std::accumulate(feature_buf.begin(), feature_buf.end(), 0.f);
    avg /= feature_buf.size();
    for (std::size_t j = 0; j < feature_buf.size(); ++j) {
        feature_buf[j] -= avg;
    }

    float variance = 0;
    for (std::size_t j = 0; j < feature_buf.size(); ++j) {
        variance += feature_buf[j] * feature_buf[j];
    }
    variance /= feature_buf.size();
    float stddev = std::sqrt(variance);
    Plog::log(Plog::LogPriority::INFO, TAG, "sigma = " + std::to_string(stddev));

    // plug into model
    int num_feats = int(feature_buf.size() / options.mel_opts.num_bins);
    at::Tensor output;
    try {
        // clang-format off
        output = pimpl->module.forward(
           std::vector<torch::jit::IValue>{
               torch::from_blob(
                   feature_buf.data(),
                   {num_feats, options.mel_opts.num_bins},
                   torch::TensorOptions().dtype(torch::kFloat32))
                   .to(pimpl->device),
        }).toTensor();
        // clang-format on
    } catch (std::runtime_error const& e) {
        Plog::log(Plog::LogPriority::ERR, TAG, "Torchscript Error");
        return "";
    }

    // convert output to string
    for (int j = 0; j < output.size(0); ++j) {
        int ch = output[j].item().toInt();
        if (ch == 2) {
            break;
        }
        str.push_back(id_to_char.at(ch));
    }

    // filter out garbage outputs
    if (str.size() > 50) {
        return "";
    }

    std::string u8str;
    utf::stringview(str.begin(), str.end()).to<utf::utf8>(std::back_inserter(u8str));

    Plog::log(Plog::LogPriority::INFO, TAG, "Extracted = " + u8str);
    using namespace std::chrono;
    auto elapsed = duration_cast<milliseconds>(system_clock::now() - pimpl->start_time);
    pimpl->log_file << elapsed.count() << ": " << u8str << "\n";

    return u8str;
}
