#include "asrmodel.h"

// torch
#include <torch/script.h>
#include <torch/torch.h>

// logging
#include <pluglog.h>

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
          torch::jit::load(model_path),
          kaldi::OnlineFbank(options),
      }),
      input_size(options.frame_opts.PaddedWindowSize()),
      feats_length(306) {
    Plog::log(Plog::LogPriority::INFO, TAG, "Torch module loaded successfully");
    input_buf.reserve(input_size);
    feature.resize(options.mel_opts.num_bins);
} catch (const c10::Error& e) {
    Plog::log(Plog::LogPriority::ERR, TAG, "Could not load torch model");
    throw;
}

ASRModel::~ASRModel()
{
}

std::u32string ASRModel::process(int16_t* buf, int size)
{
    // convert buf into mel spectrum
    input_buf.resize(size);
    for (int i = 0; i < size; ++i) {
        input_buf[i] = static_cast<float>(buf[i]) / 32767.f;
    }
    pimpl->fbank.AcceptWaveform(16000, input_buf);

    for (int i = 0; i < pimpl->fbank.NumFramesReady(); ++i) {
        pimpl->fbank.GetFrame(i, &feature);
        // TODO: feed into asr model
    }

    std::vector<torch::jit::IValue> inputs;
    inputs.push_back(torch::zeros({306, 80}));

    auto output = pimpl->module.forward(inputs).toTensor();

    std::u32string str;
    for (int i = 0; i < output.size(0); ++i) {
        int ch = output[i].item().toInt();
        str.push_back(id_to_char.at(ch));
        if (ch == 2) {
            break;
        }
    }

    Plog::log(Plog::LogPriority::INFO, TAG, u32_to_str(str));

    return str;
}
