#include <stdexcept>
#include "base/logging.h"
#include "model_rknn.h"

namespace aibox::model {

Model_RKNN::Model_RKNN() = default;

Model_RKNN::~Model_RKNN() {
    for (auto input_mem : input_mems) {
        if (input_mem) rknn_destroy_mem(ctx, input_mem);
    }
    for (auto output_mem : output_mems) {
        if (output_mem) rknn_destroy_mem(ctx, output_mem);
    }
    if (ctx) rknn_destroy(ctx);
}

void Model_RKNN::Load(std::span<u8> data) {
    const auto check_result = [](int ret, const char* msg) {
        if (ret == RKNN_SUCC) return;
        throw std::runtime_error(fmt::format("{}: {}", msg, ret));
    };

    int ret = rknn_init(&ctx, data.data(), data.size_bytes(), 0, nullptr);
    check_result(ret, "Error loading model");

    rknn_input_output_num io_num;
    ret = rknn_query(ctx, RKNN_QUERY_IN_OUT_NUM, &io_num, sizeof(io_num));
    check_result(ret, "Error querying input/output number");

    input_attrs.resize(io_num.n_input);
    output_attrs.resize(io_num.n_output);
    input_mems.resize(io_num.n_input);
    output_mems.resize(io_num.n_output);
    input_infos.resize(io_num.n_input);
    output_infos.resize(io_num.n_output);

    for (uint32_t i = 0; i < io_num.n_input; i++) {
        input_attrs[i].index = i;
        ret = rknn_query(ctx, RKNN_QUERY_INPUT_ATTR, &input_attrs[i], sizeof(rknn_tensor_attr));
        check_result(ret, "Error querying input attr");
    }
    for (uint32_t i = 0; i < io_num.n_output; i++) {
        output_attrs[i].index = i;
        ret = rknn_query(ctx, RKNN_QUERY_OUTPUT_ATTR, &output_attrs[i], sizeof(rknn_tensor_attr));
        check_result(ret, "Error querying input attr");
    }
}

void Model_RKNN::Apply() {
    for (int i = 0; i < input_attrs.size(); ++i) {
        const auto& attr = input_attrs[i];
        input_mems[i] = rknn_create_mem(ctx, attr.size_with_stride);
        rknn_set_io_mem(ctx, input_mems[i], &input_attrs[i]);
        input_infos[i] = {
                .type = ConvertTensorType(attr.type),
                .format = ConvertTensorFormat(attr.fmt),
                .shape = std::to_array(attr.dims),
                .data = {static_cast<u8*>(input_mems[i]->virt_addr), input_mems[i]->size},
                .fl = attr.fl,
                .zp = attr.zp,
                .scale = attr.scale,
        };
    }
    for (int i = 0; i < output_attrs.size(); ++i) {
        const auto& attr = output_attrs[i];
        output_mems[i] = rknn_create_mem(ctx, attr.n_elems * GetTensorSize(attr.type));
        rknn_set_io_mem(ctx, output_mems[i], &output_attrs[i]);
        output_infos[i] = {
                .type = ConvertTensorType(attr.type),
                .format = ConvertTensorFormat(attr.fmt),
                .shape = std::to_array(attr.dims),
                .data = {static_cast<u8*>(output_mems[i]->virt_addr), output_mems[i]->size},
                .fl = attr.fl,
                .zp = attr.zp,
                .scale = attr.scale,
        };
    }
}

void Model_RKNN::SetInTensorType(size_t index, TensorType type) {
    input_attrs[index].type = ConvertRknnTensorType(type);
}

void Model_RKNN::SetOutTensorType(size_t index, TensorType type) {
    output_attrs[index].type = ConvertRknnTensorType(type);
}

void Model_RKNN::Inference() {
    int ret = rknn_run(ctx, nullptr);
    if (ret != RKNN_SUCC) {
        throw std::runtime_error(fmt::format("Error running model: {}", ret));
    }
}

void Model_RKNN::SetCoreIndex(int index) {
    int core_mask = 1 << index;
    int ret = rknn_set_core_mask(ctx, static_cast<rknn_core_mask>(core_mask));
    if (ret != RKNN_SUCC) {
        throw std::runtime_error(fmt::format("Error setting core: {}", ret));
    }
}

const TensorInfo& Model_RKNN::GetInTensorInfo(size_t index) const {
    ASSERT(index < input_attrs.size());
    return input_infos[index];
}

const TensorInfo& Model_RKNN::GetOutTensorInfo(size_t index) const {
    ASSERT(index < output_attrs.size());
    return output_infos[index];
}

u32 Model_RKNN::GetTensorSize(rknn_tensor_type type) {
    switch (type) {
        case RKNN_TENSOR_INT8:
        case RKNN_TENSOR_UINT8:
            return 1;
        case RKNN_TENSOR_INT16:
        case RKNN_TENSOR_FLOAT16:
        case RKNN_TENSOR_UINT16:
            return 2;
        case RKNN_TENSOR_FLOAT32:
        case RKNN_TENSOR_INT32:
        case RKNN_TENSOR_UINT32:
        case RKNN_TENSOR_INT64:
            return 8;
        case RKNN_TENSOR_BOOL:
            return 1;
        default:
            throw std::runtime_error(
                    fmt::format("Unknown tensor type: {}", static_cast<int>(type)));
    }
}

TensorType Model_RKNN::ConvertTensorType(rknn_tensor_type type) {
    switch (type) {
        case RKNN_TENSOR_UINT8:
            return TensorType::Uint8;
        case RKNN_TENSOR_INT8:
            return TensorType::Int8;
        case RKNN_TENSOR_FLOAT16:
            return TensorType::Float16;
        case RKNN_TENSOR_FLOAT32:
            return TensorType::Float32;
        default:
            throw std::runtime_error(
                    fmt::format("Unsupported tensor type: {}", static_cast<int>(type)));
    }
}

rknn_tensor_type Model_RKNN::ConvertRknnTensorType(TensorType type) {
    switch (type) {
        case TensorType::Uint8:
            return RKNN_TENSOR_UINT8;
        case TensorType::Int8:
            return RKNN_TENSOR_INT8;
        case TensorType::Float16:
            return RKNN_TENSOR_FLOAT16;
        case TensorType::Float32:
            return RKNN_TENSOR_FLOAT32;
        default:
            throw std::runtime_error("Unknown tensor type");
    }
}

rknn_tensor_format Model_RKNN::ConvertRknnTensorFormat(TensorFormat format) {
    switch (format) {
        case TensorFormat::NHWC:
            return RKNN_TENSOR_NHWC;
        case TensorFormat::NCHW:
            return RKNN_TENSOR_NCHW;
        default:
            throw std::runtime_error("Unknown tensor format");
    }
}

TensorFormat Model_RKNN::ConvertTensorFormat(rknn_tensor_format format) {
    switch (format) {
        case RKNN_TENSOR_NHWC:
            return TensorFormat::NHWC;
        case RKNN_TENSOR_NCHW:
            return TensorFormat::NCHW;
        default:
            throw std::runtime_error("Unknown tensor format");
    }
}

}  // namespace aibox::model