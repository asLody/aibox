#pragma once

#include <string>
#include <vector>
#include <rknn_api.h>
#include "core/cv/model.h"

namespace aibox::cv {

class Model_RKNN : public Model {
public:
    Model_RKNN();

    ~Model_RKNN() override;

    void Load(const std::string& model_path) override;

    void Inference() override;

private:
    rknn_context ctx{};
    std::vector<rknn_tensor_attr> input_attrs;
    std::vector<rknn_tensor_attr> output_attrs;
    std::vector<rknn_tensor_mem*> input_mems;
    std::vector<rknn_tensor_mem*> output_mems;

    static size_t GetTensorSize(rknn_tensor_type type);
};

}  // namespace aibox::cv