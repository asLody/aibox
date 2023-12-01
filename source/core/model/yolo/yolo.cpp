#include <algorithm>
#include <cmath>
#include <numeric>
#include "core/model/yolo/yolo.h"
#include "core/model/yolo/yolo_v6.h"

namespace aibox::model {

std::shared_ptr<Yolo> Yolo::Create(YoloVersion version, std::span<u8> data) {
    switch (version) {
        case YoloVersion::YoloV6:
            return std::make_shared<YoloV6>(data);
        default:
            throw std::runtime_error("Unsupported Yolo version");
    }
}

Yolo::Yolo(std::span<u8> data) {
    model = Model::Create();
    model->Load(data);
    model->SetInTensorType(0, TensorType::Uint8);
    model->Apply();
}

void Yolo::SetCoreIndex(int index) { model->SetCoreIndex(index); }

std::pair<u32, u32> Yolo::GetInputSize() const {
    const auto& image_shape = model->GetInTensorInfo(0).shape;
    return {image_shape[1], image_shape[2]};
}

float Yolo::CalculateOverlap(float xmin0,
                             float ymin0,
                             float xmax0,
                             float ymax0,
                             float xmin1,
                             float ymin1,
                             float xmax1,
                             float ymax1) {
    const float w = fmaxf(0.f, fminf(xmax0, xmax1) - fmaxf(xmin0, xmin1) + 1.f);
    const float h = fmaxf(0.f, fminf(ymax0, ymax1) - fmaxf(ymin0, ymin1) + 1.f);
    const float i = w * h;
    const float u = (xmax0 - xmin0 + 1.f) * (ymax0 - ymin0 + 1.f) +
                    (xmax1 - xmin1 + 1.f) * (ymax1 - ymin1 + 1.f) - i;
    return u <= 0.f ? 0.f : (i / u);
}

void Yolo::DoNonMaxSuppression() {
    for (u32 i = 0; i < boxes.size(); i++) {
        const Box& n = boxes[i];
        if (n.class_id < 0) continue;

        for (u32 j = i + 1; j < boxes.size(); j++) {
            Box& m = boxes[j];
            if (m.class_id < 0) continue;

            if (n.class_id != m.class_id) continue;

            const float iou = CalculateOverlap(n.x1, n.y1, n.x2, n.y2, m.x1, m.y1, m.x2, m.y2);
            if (iou >= threshold_nms) {
                m.class_id = -1;
            }
        }
    }
}

void Yolo::Detect(std::vector<Box>& boxes_out, std::span<u8> image) {
    memcpy(model->GetInTensorInfo(0).data.data(), image.data(), image.size_bytes());
    model->Inference();

    PostProcess();

    std::sort(boxes.begin(), boxes.end(), [](const Box& b1, const Box& b2) {
        return b1.score > b2.score;
    });
    DoNonMaxSuppression();

    for (auto& box : boxes) {
        if (box.class_id < 0) continue;
        if (box.score < threshold) {
            continue;
        }
        boxes_out.push_back(box);
    }
    boxes.clear();
}

}  // namespace aibox::model
