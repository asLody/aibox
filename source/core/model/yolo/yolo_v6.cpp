#include <algorithm>
#include <numeric>
#include <optional>
#include "core/model/yolo/yolo_v6.h"

namespace aibox::model {

YoloV6::YoloV6(const std::string& model_path) : Yolo(model_path) {
    score_sum_available = model->GetOutTensorCount() > 6;
    if (const u32 len = model->GetOutTensorInfo(0).shape[1]; len != 4) {
        dfl_len = model->GetOutTensorInfo(0).shape[1] / 4;
    }
}

inline static s32 Clamp(float val) {
    float res = val <= -128.f ? -128.f : (val >= 127.f ? 127.f : val);
    return (s32)res;
}

inline static int8_t F32ToAffine(float f32, s32 zp, float scale) {
    float dst_val = (f32 / scale) + (float)zp;
    s8 res = (s8)Clamp(dst_val);
    return res;
}

inline static float AffineToF32(int8_t qnt, int32_t zp, float scale) {
    return ((float)qnt - (float)zp) * scale;
}

void YoloV6::PostProcess() {
    for (size_t i = 0; i < 3; ++i) {
        size_t nt_per_pair = 2;

        const TensorInfo* tensor_score_sum{};

        if (score_sum_available) {
            nt_per_pair = 3;
            tensor_score_sum = &model->GetOutTensorInfo(i * nt_per_pair + 2);
        }

        const auto& image_shape = model->GetInTensorInfo(0).shape;
        const auto& box_shape = model->GetOutTensorInfo(i * nt_per_pair).shape;
        const u32 grid_h = box_shape[2];
        const u32 grid_w = box_shape[3];
        const u32 stride = image_shape[1] / grid_h;

        ProcessData(&model->GetOutTensorInfo(i * nt_per_pair),
                    &model->GetOutTensorInfo(i * nt_per_pair + 1),
                    tensor_score_sum,
                    grid_h,
                    grid_w,
                    static_cast<float>(stride));
    }
}

void YoloV6::ProcessData(const TensorInfo* tensor_box,
                         const TensorInfo* tensor_score,
                         const TensorInfo* tensor_score_sum,
                         u32 grid_h,
                         u32 grid_w,
                         float stride) {
    constexpr float sum_noise = kYoloClassNum * 1.f / 256;

    const u32 grid_len = grid_h * grid_w;

    const s8 zero_i8 = F32ToAffine(0.f, tensor_score->zp, tensor_score->scale);
    const s8 threshold_i8 = F32ToAffine(threshold, tensor_score->zp, tensor_score->scale);
    const s8 sum_noise_i8 = F32ToAffine(sum_noise, tensor_score_sum->zp, tensor_score_sum->scale);

    for (u32 i = 0; i < grid_h; i++) {
        for (u32 j = 0; j < grid_w; j++) {
            s32 max_class_id = -1;
            u32 offset = i * grid_w + j;

            if (score_sum_available) {
                if (tensor_score_sum->Data()[offset] < sum_noise_i8) {
                    continue;
                }
            }
            s8 max_score = zero_i8;
            for (s32 c = 0; c < kYoloClassNum; ++c) {
                const s8 score = tensor_score->Data()[offset];
                if (score > threshold_i8 && score > max_score) {
                    max_score = score;
                    max_class_id = c;
                }
                offset += grid_len;
            }

            if (max_score > threshold_i8) {
                offset = i * grid_w + j;
                std::array<float, 4> box{};
                if (dfl_len > 1) {
                    abort();
                } else if (dfl_len < 1) {
                    for (u32 k = 0; k < 4; k++) {
                        box[k] = AffineToF32(
                                tensor_box->Data()[offset], tensor_box->zp, tensor_box->scale);
                        offset += grid_len;
                    }
                }

                const float x1 = (-box[0] + static_cast<float>(j) + 0.5f) * stride;
                const float y1 = (-box[1] + static_cast<float>(i) + 0.5f) * stride;
                const float x2 = (box[2] + static_cast<float>(j) + 0.5f) * stride;
                const float y2 = (box[3] + static_cast<float>(i) + 0.5f) * stride;
                const float score = AffineToF32(max_score, tensor_score->zp, tensor_score->scale);

                boxes.push_back({
                        max_class_id,
                        score,
                        x1,
                        y1,
                        x2,
                        y2,
                });
            }
        }
    }
}

}  // namespace aibox::model