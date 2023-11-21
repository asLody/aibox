#include <stdexcept>
#include "base/logging.h"
#include "model_rknn.h"

namespace aibox::cv {

Model_RKNN::Model_RKNN() = default;

Model_RKNN::~Model_RKNN() {
    for (auto input_mem : input_mems) {
        rknn_destroy_mem(ctx, input_mem);
    }
    for (auto output_mem : output_mems) {
        rknn_destroy_mem(ctx, output_mem);
    }
    rknn_destroy(ctx);
}

void Model_RKNN::Load(const std::string& model_path) {
    const auto check_result = [](int ret, const char* msg) {
        if (!ret) return;
        throw std::runtime_error(fmt::format("{}: {}", msg, ret));
    };

    int ret = rknn_init(&ctx, (void*)model_path.c_str(), 0, 0, nullptr);
    check_result(ret, "Error loading model");

    rknn_sdk_version sdk_ver;
    ret = rknn_query(ctx, RKNN_QUERY_SDK_VERSION, &sdk_ver, sizeof(sdk_ver));
    check_result(ret, "Error querying SDK version");
    LOG_INFO("RKNN SDK version: {}.{}", sdk_ver.api_version, sdk_ver.drv_version);

    rknn_input_output_num io_num;
    ret = rknn_query(ctx, RKNN_QUERY_IN_OUT_NUM, &io_num, sizeof(io_num));
    check_result(ret, "Error querying input/output number");
    LOG_INFO("Model input number: {}, output number: {}", io_num.n_input, io_num.n_output);

    input_attrs.resize(io_num.n_input);
    output_attrs.resize(io_num.n_output);

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

    rknn_custom_string custom_string;
    ret = rknn_query(ctx, RKNN_QUERY_CUSTOM_STRING, &custom_string, sizeof(custom_string));
    check_result(ret, "Error querying custom string");
    LOG_INFO("Model custom string: {}", custom_string.string);

    input_mems.resize(io_num.n_input);
    output_mems.resize(io_num.n_output);
    for (int i = 0; i < input_mems.size(); ++i) {
        input_mems[i] = rknn_create_mem(
                ctx, input_attrs[i].size_with_stride * GetTensorSize(input_attrs[i].type));
        rknn_set_io_mem(ctx, input_mems[i], &input_attrs[i]);
    }
    for (int i = 0; i < output_mems.size(); ++i) {
        output_mems[i] = rknn_create_mem(
                ctx, output_attrs[i].size_with_stride * GetTensorSize(input_attrs[i].type));
        rknn_set_io_mem(ctx, output_mems[i], &output_attrs[i]);
    }
}

size_t Model_RKNN::GetTensorSize(rknn_tensor_type type) {
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
            throw std::runtime_error("Unknown tensor type");
    }
}

void Model_RKNN::Inference() {
    int ret = rknn_run(ctx, nullptr);
    if (ret != 0) {
        throw std::runtime_error(fmt::format("Error running model: {}", ret));
    }
}

}  // namespace aibox::cv