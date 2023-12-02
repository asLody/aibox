#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include "base/fps_counter.h"
#include "base/macros.h"
#include "base/pid.h"
#include "base/wsq.h"

namespace aibox::video {
class VideoCapture;
}  // namespace aibox::video

namespace aibox::aimbot {

struct FrameResource;
struct NPUResource;

using MoveCallback = std::function<void(float x, float y)>;

class AimbotFPS {
public:
    AimbotFPS();

    ~AimbotFPS();

    void Run();

    void Stop();

    void LoadModel(const std::string& model_path);

    void SetMoveCallback(MoveCallback callback) { move_callback = std::move(callback); }

private:
    u32 npu_count{3};
    float smooth_x{0.4}, smooth_y{0.4};

    base::PID pid_x{1.6, 0, 0.15, -65, 65, 0.05};
    base::PID pid_y{1.6, 0, 0.15, -65, 65, 0.05};

    base::FPSCounter counter;
    std::unique_ptr<video::VideoCapture> video_capture;
    std::vector<std::shared_ptr<NPUResource>> npu_resources;
    std::atomic_bool stop_requested{};
    u32 in_width{}, in_height{};
    u32 resize_width{}, resize_height{};
    u32 out_width{640}, out_height{640};
    float scaled_x{}, scaled_y{};
    float center_x{}, center_y{};
    std::vector<std::shared_ptr<FrameResource>> frame_resources;
    std::atomic<u32> next_npu_core{};
    MoveCallback move_callback{};

    std::mutex npu_work_mutex;
    std::condition_variable npu_work_cv;
    std::vector<std::thread> npu_threads;
    WorkStealingQueue<u32> npu_work_queue;

    ALWAYS_INLINE u32 NextNPUCoreIndex() {
        return next_npu_core.fetch_add(1, std::memory_order_relaxed) % npu_resources.size();
    }

    void CreateWorkerThreads();

    void SetupVideoInput();

    void TickFrame(u32 buffer_index);

    void PreProcess(FrameResource* frame_resource) const;
};

}  // namespace aibox::aimbot