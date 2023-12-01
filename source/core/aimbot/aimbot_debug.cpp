#define cimg_display 0
#include <CImg.h>
#include "core/aimbot/aimbot_debug.h"

namespace aibox::aimbot {

constexpr std::array COCO_LABELS = {"person",        "bicycle",      "car",
                               "motorcycle",    "airplane",     "bus",
                               "train",         "truck",        "boat",
                               "traffic light", "fire hydrant", "stop sign",
                               "parking meter", "bench",        "bird",
                               "cat",           "dog",          "horse",
                               "sheep",         "cow",          "elephant",
                               "bear",          "zebra",        "giraffe",
                               "backpack",      "umbrella",     "handbag",
                               "tie",           "suitcase",     "frisbee",
                               "skis",          "snowboard",    "sports ball",
                               "kite",          "baseball bat", "baseball glove",
                               "skateboard",    "surfboard",    "tennis racket",
                               "bottle",        "wine glass",   "cup",
                               "fork",          "knife",        "spoon",
                               "bowl",          "banana",       "apple",
                               "sandwich",      "orange",       "broccoli",
                               "carrot",        "hot dog",      "pizza",
                               "donut",         "cake",         "chair",
                               "couch",         "potted plant", "bed",
                               "dining table",  "toilet",       "tv",
                               "laptop",        "mouse",        "remote",
                               "keyboard",      "cell phone",   "microwave",
                               "oven",          "toaster",      "sink",
                               "refrigerator",  "book",         "clock",
                               "vase",          "scissors",     "teddy bear",
                               "hair drier",    "toothbrush"};

void WriteMarkedImage(const std::string& path,
                      const u8* data,
                      u32 width,
                      u32 height,
                      const std::vector<model::Box>& boxes,
                      s32 target_index) {
    cimg_library::CImg<u8> image(width, height, 1, 3);
    for (u32 i = 0; i < width * height; ++i) {
        image(i % width, i / width, 0) = data[i * 3 + 2];
        image(i % width, i / width, 1) = data[i * 3 + 1];
        image(i % width, i / width, 2) = data[i * 3 + 0];
    }

    for (int i = 0; i < boxes.size(); ++i) {
        const auto& box = boxes[i];
        constexpr unsigned char COLOR_RED[] = {210, 0, 0};
        constexpr unsigned char COLOR_GREEN[] = {0, 210, 0};
        const bool is_target = i == target_index;
        image.draw_rectangle(static_cast<int>(box.x1),
                             static_cast<int>(box.y1),
                             static_cast<int>(box.x2),
                             static_cast<int>(box.y2),
                             is_target ? COLOR_GREEN : COLOR_RED,
                             1,
                             ~0u);
        image.draw_text(static_cast<int>(box.x1) + 5,
                        static_cast<int>(box.y1),
                        COCO_LABELS[box.class_id],
                        is_target ? COLOR_GREEN : COLOR_RED);
    }
    image.save_bmp(path.c_str());
}

}  // namespace aibox::aimbot
