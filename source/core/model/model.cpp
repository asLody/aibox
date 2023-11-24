#include "core/model/model.h"
#include "core/model/rknn/model_rknn.h"

namespace aibox::model {

Model::Model() = default;

Model::~Model() = default;

std::shared_ptr<Model> Model::Create() {
    return std::make_shared<Model_RKNN>();
}

}  // namespace aibox::cv