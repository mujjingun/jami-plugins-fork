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

    std::u32string process(std::int16_t* buf, int size);

private:
    std::unique_ptr<ASRModelPimpl> pimpl;
    int input_size, feats_length;
    std::vector<float> input_buf;
    std::vector<float> feature;
};

#endif // ASRMODEL_H
