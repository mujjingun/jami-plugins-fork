#include "asrmodel.h"

// torch
#include <torch/script.h>
#include <torch/torch.h>

// logging
#include <pluglog.h>

#include <algorithm>
#include <cuchar>

#include "codes.h"
#include "feat/online-feature.h"

static const std::string TAG = "AutoSub";

static const kaldi::FbankOptions options = [] {
    kaldi::FbankOptions options;
    options.mel_opts.num_bins = 80;
    options.frame_opts.frame_length_ms = 20.0f;
    options.frame_opts.frame_shift_ms = 10.0f;
    options.frame_opts.window_type = "hamming";
    return options;
}();

struct ASRModelPimpl {
    torch::Device device;
    torch::jit::script::Module module;
    kaldi::OnlineFbank fbank;
};

static std::string u32_to_str(std::u32string const& str)
{
    std::string result{};
    std::mbstate_t state{};
    char out[MB_LEN_MAX]{};
    for (char32_t c : str) {
        std::size_t rc = std::c32rtomb(out, c, &state);
        if (rc != std::size_t(-1)) {
            result += std::string_view{out, rc};
        }
    }
    return result;
}

ASRModel::ASRModel(std::string const& model_path) try
    : pimpl(new ASRModelPimpl{
          torch::Device(torch::kCPU),
          torch::jit::load(model_path),
          kaldi::OnlineFbank(options),
      }),
      input_size(options.frame_opts.PaddedWindowSize()),
      feats_length(1000) // 10s of audio
{
    pimpl->module.to(pimpl->device);
    Plog::log(Plog::LogPriority::INFO, TAG, "Torch module loaded successfully");

    input_buf.reserve(input_size);
    feature.resize(options.mel_opts.num_bins);
    feature_buf.reserve(options.mel_opts.num_bins * feats_length);
} catch (const c10::Error& e) {
    Plog::log(Plog::LogPriority::ERR, TAG, "Could not load torch model");
    throw;
}

ASRModel::~ASRModel()
{
}

std::u32string ASRModel::process(int16_t* buf, int size)
{
    torch::NoGradGuard no_grad;

    // convert buf into mel spectrum
    input_buf.resize(size);
    for (int i = 0; i < size; ++i) {
        input_buf[i] = static_cast<float>(buf[i]) / 32767.f;
    }
    pimpl->fbank.AcceptWaveform(16000, input_buf);

    std::u32string str;

    for (int i = 0; i < pimpl->fbank.NumFramesReady(); ++i) {
        pimpl->fbank.GetFrame(i, &feature);
        std::copy(feature.begin(), feature.end(), std::back_inserter(feature_buf));

        if (feature_buf.size() == std::size_t(options.mel_opts.num_bins * feats_length)) {
            float avg = std::accumulate(feature_buf.begin(), feature_buf.end(), 0.f);
            avg /= feature_buf.size();
            for (std::size_t j = 0; j < feature_buf.size(); ++j) {
                feature_buf[j] -= avg;
            }

            // clang-format off
            auto output = pimpl->module.forward(
               std::vector<torch::jit::IValue>{
                   torch::from_blob(
                       feature_buf.data(),
                       {feats_length, options.mel_opts.num_bins},
                       torch::TensorOptions().dtype(torch::kFloat32))
                       .to(pimpl->device),
            }).toTensor();
            // clang-format on

            // convert output to string
            for (int j = 0; j < output.size(0); ++j) {
                int ch = output[j].item().toInt();
                if (ch == 2) {
                    break;
                }
                str.push_back(id_to_char.at(ch));
            }

            feature_buf.clear();
        }
    }

    std::cout << u32_to_str(str) << "\n";

    return str;
}
