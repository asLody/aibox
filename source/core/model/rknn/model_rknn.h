#pragma once

#include <string>
#include <vector>
#include <rknn_api.h>
#include "core/model/model.h"

namespace aibox::model {

class Model_RKNN : public Model {
public:
    Model_RKNN();

    ~Model_RKNN() override;

    void SetCoreIndex(int index) override;

    void Load(const std::string& model_path) override;

    void Apply() override;

    void Inference() override;

    void SetInTensorType(size_t index, TensorType type) override;

    void SetOutTensorType(size_t index, TensorType type) override;

    [[nodiscard]] size_t GetInTensorCount() const override { return input_infos.size(); }

    [[nodiscard]] size_t GetOutTensorCount() const override { return output_infos.size(); }

    [[nodiscard]] const TensorInfo& GetInTensorInfo(size_t index) const override;

    [[nodiscard]] const TensorInfo& GetOutTensorInfo(size_t index) const override;

private:
    rknn_context ctx{};
    std::vector<rknn_tensor_attr> input_attrs;
    std::vector<rknn_tensor_attr> output_attrs;
    std::vector<TensorInfo> input_infos;
    std::vector<TensorInfo> output_infos;
    std::vector<rknn_tensor_mem*> input_mems;
    std::vector<rknn_tensor_mem*> output_mems;

    static u32 GetTensorSize(rknn_tensor_type type);

    static TensorType ConvertTensorType(rknn_tensor_type type);

    static rknn_tensor_type ConvertRknnTensorType(TensorType type);

    static rknn_tensor_format ConvertRknnTensorFormat(TensorFormat format);

    static TensorFormat ConvertTensorFormat(rknn_tensor_format format);
};

}  // namespace aibox::model