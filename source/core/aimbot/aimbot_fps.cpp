#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>
#include <im2d.hpp>
#include "base/alignment.h"
#include "base/file_utils.h"
#include "core/aimbot/aimbot_debug.h"
#include "core/aimbot/aimbot_fps.h"
#include "core/model/yolo/yolo.h"
#include "core/video/video_capture.h"

namespace aibox::aimbot {

struct FrameResource {
    video::VideoBuffer buffer{};
    rga_buffer_handle_t in_handle{};
    rga_buffer_t in_img{};
    rga_buffer_handle_t out_handle{};
    std::vector<u8> resize_data{};
    rga_buffer_t resize_img{};
    rga_buffer_handle_t resize_handle{};
    rga_buffer_t out_img{};
    std::vector<u8> out_data;

    FrameResource(const video::VideoBuffer& in_buffer,
                  u32 in_width,
                  u32 in_height,
                  u32 resize_width,
                  u32 resize_height,
                  u32 out_width,
                  u32 out_height) {
        buffer = in_buffer;
        in_handle = importbuffer_virtualaddr(buffer.buffer, static_cast<int>(buffer.length));
        in_img = wrapbuffer_handle(in_handle,
                                   static_cast<int>(in_width),
                                   static_cast<int>(in_height),
                                   RK_FORMAT_BGR_888);
        resize_data.resize(resize_width * resize_height * 3);
        resize_handle =
                importbuffer_virtualaddr(resize_data.data(), static_cast<int>(resize_data.size()));
        resize_img = wrapbuffer_handle(resize_handle,
                                       static_cast<int>(resize_width),
                                       static_cast<int>(resize_height),
                                       RK_FORMAT_BGR_888);
        out_data.resize(out_width * out_height * 3);
        out_handle = importbuffer_virtualaddr(out_data.data(), static_cast<int>(out_data.size()));
        out_img = wrapbuffer_handle(out_handle,
                                    static_cast<int>(out_width),
                                    static_cast<int>(out_height),
                                    RK_FORMAT_RGB_888);
    }

    ~FrameResource() {
        if (in_handle) {
            releasebuffer_handle(in_handle);
        }
        if (resize_handle) {
            releasebuffer_handle(resize_handle);
        }
        if (out_handle) {
            releasebuffer_handle(out_handle);
        }
    }
};

struct NPUResource {
    std::shared_ptr<model::Yolo> model;
    std::vector<model::Box> boxes;
};

AimbotFPS::AimbotFPS() {
    video_capture = std::make_unique<video::VideoCapture>();
    CreateWorkerThreads();
    video_capture->Open();
}

AimbotFPS::~AimbotFPS() {
    stop_requested = true;
    std::scoped_lock lock(npu_work_mutex);
    npu_work_cv.notify_all();
    for (int i = 0; i < npu_threads.size(); ++i) {
        npu_threads[i].join();
    }
}

void AimbotFPS::Run() {
    SetupVideoInput();
    video_capture->StartStreaming();
    counter.Reset();
    while (!stop_requested) {
        u32 buffer_index = video_capture->DequeueBuffer();
        npu_work_queue.push(buffer_index);
        {
            std::scoped_lock lock(npu_work_mutex);
            npu_work_cv.notify_one();
        }
        counter.Tick();
        printf("FPS: %.2f\n", counter.GetFPS());
    }
    video_capture->StopStreaming();
}

void AimbotFPS::Stop() { stop_requested = true; }

void AimbotFPS::SetupVideoInput() {
    in_width = video_capture->GetWidth();
    in_height = video_capture->GetHeight();

    const float scale = static_cast<float>(out_height) * 1.5f / static_cast<float>(in_height);
    resize_width = base::AlignUp(static_cast<u32>(static_cast<float>(in_width) * scale), 128);
    resize_height = base::AlignUp(static_cast<u32>(static_cast<float>(in_height) * scale), 128);

    scaled_x = 1920.f / static_cast<float>(in_width);
    scaled_y = 1080.f / static_cast<float>(in_height);

    const auto& buffers = video_capture->GetBuffers();
    frame_resources.resize(buffers.size());
    for (int i = 0; i < buffers.size(); i++) {
        frame_resources[i] = std::make_shared<FrameResource>(buffers[i],
                                                             in_width,
                                                             in_height,
                                                             resize_width,
                                                             resize_height,
                                                             out_width,
                                                             out_height);
    }
    LOG_INFO("In: {}x{} -> Resize: {}x{} -> Out: {}x{}",
             in_width,
             in_height,
             resize_width,
             resize_height,
             out_width,
             out_height);
}

void AimbotFPS::LoadModel(const std::string& model_path) {
    npu_resources.resize(npu_count);
    auto model_data = base::ReadFile(model_path);
    for (int i = 0; i < npu_resources.size(); ++i) {
        npu_resources[i] = std::make_shared<NPUResource>();
        npu_resources[i]->model = model::Yolo::Create(model::YoloVersion::YoloV6, model_data);
        npu_resources[i]->model->SetCoreIndex(i);
    }
    std::tie(out_width, out_height) = npu_resources[0]->model->GetInputSize();
    ASSERT(out_width == out_height);
    center_x = static_cast<float>(out_width) / 2.f;
    center_y = static_cast<float>(out_height) / 2.f;
}

void AimbotFPS::TickFrame(u32 buffer_index) {
    const u32 core_index = NextNPUCoreIndex();
    auto& npu_resource = npu_resources[core_index];
    npu_resource->boxes.clear();
    auto& frame_resource = frame_resources[buffer_index];
    PreProcess(frame_resource.get());
    video_capture->RequeueBuffer(buffer_index);
    npu_resource->model->Detect(npu_resource->boxes, frame_resource->out_data);

    float closest_distance = -1.f;
    float closest_target_x{}, closest_target_y{};
    s32 target_box_index{-1};

    for (int i = 0; i < npu_resource->boxes.size(); ++i) {
        const auto& box = npu_resource->boxes[i];
        const float target_x = (box.x1 + box.x2) / 2.f;
        const float target_y = box.y1 + (box.y2 - box.y1) * 0.15f;
        const float distance =
                std::sqrt(std::pow(target_x - center_x, 2.f) + std::pow(target_y - center_y, 2.f));
        if (closest_distance < 0.f || distance < closest_distance) {
            closest_distance = distance;
            closest_target_x = target_x;
            closest_target_y = target_y;
            target_box_index = i;
        }
    }

    if (target_box_index >= 0) {
        const float pixel_move_x = (closest_target_x - center_x) * smooth_x * scaled_x;
        const float pixel_move_y = (closest_target_y - center_y) * smooth_y * scaled_y;
        const float mouse_mov_x = pid_x.Update(atan2f(pixel_move_x, static_cast<float>(out_width)) *
                                               static_cast<float>(out_width));
        const float mouse_mov_y =
                pid_y.Update(atan2f(pixel_move_y, static_cast<float>(out_height)) *
                             static_cast<float>(out_height));
        move_callback(mouse_mov_x, mouse_mov_y);
#if 0
        WriteMarkedImage("/tmp/aimbot.bmp",
                         frame_resource->out_data.data(),
                         out_width,
                         out_height,
                         npu_resource->boxes,
                         target_box_index);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
#endif
    }
}

void AimbotFPS::PreProcess(FrameResource* frame_resource) const {
    int ret = imresize(frame_resource->in_img, frame_resource->resize_img);
    if (ret < 0) {
        throw std::runtime_error("imresize failed");
    }

    ret = imcrop(frame_resource->resize_img,
                 frame_resource->out_img,
                 {
                         .x = static_cast<int>((resize_width - out_width) / 2),
                         .y = static_cast<int>((resize_height - out_height) / 2),
                         .width = static_cast<int>(out_width),
                         .height = static_cast<int>(out_height),
                 });
    if (ret < 0) {
        throw std::runtime_error("imcrop failed");
    }
}

void AimbotFPS::CreateWorkerThreads() {
    npu_threads.resize(npu_count);
    for (int i = 0; i < npu_count; ++i) {
        npu_threads[i] = std::thread([this]() {
            while (!stop_requested) {
                if (npu_work_queue.empty()) {
                    std::unique_lock<std::mutex> lock(npu_work_mutex);
                    npu_work_cv.wait(
                            lock, [this]() { return stop_requested || !npu_work_queue.empty(); });
                }
                const auto buffer_index = npu_work_queue.steal();
                if (!buffer_index) {
                    continue;
                }
                TickFrame(*buffer_index);
            }
        });
    }
}

}  // namespace aibox::aimbot
