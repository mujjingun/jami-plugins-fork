#ifndef ASRMODEL_H
#define ASRMODEL_H

#include <memory>
#include <string>

struct ASRModelPimpl;

class ASRModel {
public:
    ASRModel(std::string const& model_path);
    ~ASRModel();

    std::u32string process(std::int16_t* buf, int size);

private:
    std::unique_ptr<ASRModelPimpl> pimpl;
};

#endif // ASRMODEL_H
