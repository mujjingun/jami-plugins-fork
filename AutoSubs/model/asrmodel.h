#ifndef ASRMODEL_H
#define ASRMODEL_H

#include <memory>
#include <string>
#include <vector>

struct ASRModelPimpl;

class ASRModel {
public:
    ASRModel(std::string const& model_path);
    ~ASRModel();

    std::string process(std::int16_t* buf, int size);
    void reset();

private:
    std::string extract_text();

private:
    std::unique_ptr<ASRModelPimpl> pimpl;
    int input_size, max_num_feats;
    int vad_frame_length;

    int counter = 0;
    bool voice_activity = false;
    std::vector<std::int16_t> vad_input_buf;
    std::vector<float> input_buf;
    int feature_offset = 0;
    std::vector<float> feature, feature_buf;
};

#endif // ASRMODEL_H
