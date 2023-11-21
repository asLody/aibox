#pragma once

#include <string>
#include <vector>
#include <rknn_api.h>
#include "base/types.h"

namespace aibox::cv {

class Model {
public:
    Model();

    virtual ~Model();

    virtual void Load(const std::string& model_path) = 0;

    virtual void Inference() = 0;
};

}  // namespace aibox::cv