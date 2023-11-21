#include <stdexcept>
#include <vector>
#include "base/logging.h"
#include "model_rknn.h"

namespace aibox::cv {

Model_RKNN::Model_RKNN() = default;

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

    std::vector<rknn_tensor_attr> input_attrs(io_num.n_input);
    for (uint32_t i = 0; i < io_num.n_input; i++) {
        input_attrs[i].index = i;
        ret = rknn_query(ctx, RKNN_QUERY_INPUT_ATTR, &(input_attrs[i]), sizeof(rknn_tensor_attr));
        check_result(ret, "Error querying input attr");
    }

    rknn_custom_string custom_string;
    ret = rknn_query(ctx, RKNN_QUERY_CUSTOM_STRING, &custom_string, sizeof(custom_string));
    check_result(ret, "Error querying custom string");
    LOG_INFO("Model custom string: {}", custom_string.string);
}

Model_RKNN::~Model_RKNN() {
    rknn_destroy(ctx);
}

}  // namespace aibox::cv