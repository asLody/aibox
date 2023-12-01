#pragma once

#include <memory>
#include <vector>
#include "base/logging.h"
#include "base/macros.h"
#include "base/types.h"
#include "core/model/model.h"

namespace aibox::model {

constexpr size_t kYoloClassNum = 80;

struct Box {
    s32 class_id;
    float score;
    float x1, y1, x2, y2;
};

enum class YoloVersion : u32 {
    YoloV5,
    YoloV6,
    YoloV7,
    YoloV8,
};

class Yolo {
public:
    explicit Yolo(std::span<u8> data);

    static std::shared_ptr<Yolo> Create(YoloVersion version, std::span<u8> data);

    void SetThreshold(float val) { threshold = val; }

    void SetCoreIndex(int index);

    void Detect(std::vector<Box>& boxes_out, std::span<u8> image);

    [[nodiscard]] std::pair<u32, u32> GetInputSize() const;

protected:
    float threshold{0.4f};
    float threshold_nms{0.45f};
    std::shared_ptr<Model> model;
    std::vector<Box> boxes;

    void DoNonMaxSuppression();

    virtual void PostProcess() = 0;

    ALWAYS_INLINE static float CalculateOverlap(float xmin0,
                                                float ymin0,
                                                float xmax0,
                                                float ymax0,
                                                float xmin1,
                                                float ymin1,
                                                float xmax1,
                                                float ymax1);
};

}  // namespace aibox::model