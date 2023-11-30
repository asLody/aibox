#pragma once

#include <memory>
#include <span>
#include <string>
#include <rknn_api.h>
#include "base/types.h"

namespace aibox::model {

enum class TensorType : u32 {
    Uint8 = 0,
    Int8,
    Float16,
    Float32,
};

enum class TensorFormat : u32 {
    NHWC = 0,
    NCHW,
};

struct TensorInfo {
    TensorType type;
    TensorFormat format;
    std::array<u32, 16> shape;
    std::span<u8> data;
    s8 fl;
    s32 zp;
    float scale;

    template <typename T = s8> [[nodiscard]] T* Data() const {
        return reinterpret_cast<T*>(data.data());
    }
};

class Model {
public:
    static std::shared_ptr<Model> Create();

    Model();

    Model(const Model&) = delete;

    Model& operator=(const Model&) = delete;

    virtual ~Model();

    virtual void SetCoreIndex(int index) = 0;

    virtual void Load(std::span<u8> data) = 0;

    virtual void Apply() = 0;

    virtual void Inference() = 0;

    virtual void SetInTensorType(size_t index, TensorType type) = 0;

    virtual void SetOutTensorType(size_t index, TensorType type) = 0;

    [[nodiscard]] virtual const TensorInfo& GetInTensorInfo(size_t index) const = 0;

    [[nodiscard]] virtual const TensorInfo& GetOutTensorInfo(size_t index) const = 0;

    [[nodiscard]] virtual size_t GetInTensorCount() const = 0;

    [[nodiscard]] virtual size_t GetOutTensorCount() const = 0;
};

}  // namespace aibox::model