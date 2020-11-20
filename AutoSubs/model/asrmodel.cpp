#include "asrmodel.h"

// torch
#include <torch/script.h>
#include <torch/torch.h>

// logging
#include <pluglog.h>

#include <cuchar>

#include "codes.h"
#include "feat/feature-fbank.h"

static const std::string TAG = "AutoSub";

struct ASRModelPimpl {
    torch::jit::script::Module module;
    kaldi::FbankComputer fbank;
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
          kaldi::FbankComputer(kaldi::FbankOptions{

          }),
      }) {
    Plog::log(Plog::LogPriority::INFO, TAG, "Torch module loaded successfully");
} catch (const c10::Error& e) {
    Plog::log(Plog::LogPriority::ERR, TAG, "Could not load torch model");
    throw;
}

ASRModel::~ASRModel()
{
}

std::u32string ASRModel::process(int16_t* buf, int size)
{
    // TODO: convert buf into mel spectrum
    //pimpl->fbank.Compute();

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
