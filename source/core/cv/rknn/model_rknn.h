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

private:
    rknn_context ctx{};
};

}  // namespace aibox::cv