#pragma once

#include <atomic>
#include <im2d_buffer.h>
#include "base/pid.h"
#include "core/model/yolo/yolo.h"
#include "core/video/video_capture.h"

namespace aibox::aimbot {

struct FrameBuffer {
    video::VideoBuffer source;
    rga_buffer_handle_t in_handle;
    rga_buffer_t in_img;
    rga_buffer_handle_t out_handle;
    rga_buffer_t out_img;
    std::vector<u8> out_data;

    ~FrameBuffer();
};

class AimbotFPS {
public:
    AimbotFPS();

    void Run();

    void Stop();

    void LoadModel(const std::string& model_path);

    void TickFrame(u32 buffer_index);

private:
    base::PID pid_x{3.7, 0, 0.11, -50, 50, 0.05};
    base::PID pid_y{3.7, 0, 0.11, -50, 50, 0.05};

    video::VideoCapture video_capture;
    std::vector<std::shared_ptr<model::Yolo>> models;
    bool stop_requested{};
    u32 in_width{};
    u32 in_height{};
    u32 out_width{640};
    u32 out_height{640};
    std::vector<std::shared_ptr<FrameBuffer>> frame_buffers;
    std::atomic<u32> next_free_npu_core{};

    std::vector<model::Box> boxes;

    ALWAYS_INLINE u32 NextCoreIndex() {
        return next_free_npu_core.fetch_add(1, std::memory_order_relaxed) % models.size();
    }

    void SetupInput();

    void ProcessFrameData(FrameBuffer* buffer) const;
};

}  // namespace aibox::aimbot