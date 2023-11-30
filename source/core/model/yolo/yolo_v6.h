#pragma once

#include <vector>
#include "base/logging.h"
#include "base/macros.h"
#include "base/types.h"
#include "core/model/model.h"
#include "core/model/yolo/yolo.h"

namespace aibox::model {

class YoloV6 : public Yolo {
public:
    explicit YoloV6(std::span<u8> data);

private:
    bool score_sum_available{};
    u32 dfl_len{};

    void PostProcess() override;

    void ProcessData(const TensorInfo* tensor_box,
                     const TensorInfo* tensor_score,
                     const TensorInfo* tensor_score_sum,
                     u32 grid_h,
                     u32 grid_w,
                     float stride);
};

}  // namespace aibox::model