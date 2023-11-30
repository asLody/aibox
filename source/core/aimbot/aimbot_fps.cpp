#include <im2d.hpp>
#include "base/file_utils.h"
#include "core/aimbot/aimbot_fps.h"

namespace aibox::aimbot {

AimbotFPS::AimbotFPS() { video_capture.Open(); }

void AimbotFPS::Run() {
    SetupInput();
    video_capture.StartStreaming();
    while (!stop_requested) {
        u32 index = video_capture.DequeueBuffer();
        TickFrame(index);
        video_capture.RequeueBuffer(index);
    }
    video_capture.StopStreaming();
}

void AimbotFPS::Stop() { stop_requested = true; }

void AimbotFPS::SetupInput() {
    in_width = video_capture.GetWidth();
    in_height = video_capture.GetHeight();
    const auto& buffers = video_capture.GetBuffers();
    frame_buffers.resize(buffers.size());

    for (int i = 0; i < buffers.size(); i++) {
        const auto& source = buffers[i];
        auto frame_buffer = std::make_shared<FrameBuffer>();
        frame_buffer->source = source;
        frame_buffer->out_data.resize(out_width * out_height * 3);
        frame_buffer->in_handle =
                importbuffer_virtualaddr(source.buffer, static_cast<int>(source.length));
        frame_buffer->in_img = wrapbuffer_handle(frame_buffer->in_handle,
                                                 static_cast<int>(in_width),
                                                 static_cast<int>(in_height),
                                                 RK_FORMAT_BGR_888);
        frame_buffer->out_handle = importbuffer_virtualaddr(
                frame_buffer->out_data.data(), static_cast<int>(frame_buffer->out_data.size()));
        frame_buffer->out_img = wrapbuffer_handle(frame_buffer->out_handle,
                                                  static_cast<int>(out_width),
                                                  static_cast<int>(out_height),
                                                  RK_FORMAT_RGB_888);
        frame_buffers[i] = frame_buffer;
    }
}

void AimbotFPS::LoadModel(const std::string& model_path) {
    models.resize(3);
    auto model_data = base::ReadFile(model_path);
    for (int i = 0; i < models.size(); ++i) {
        models[i] = model::Yolo::Create(model::YoloVersion::YoloV6, model_data);
        models[i]->SetCoreIndex(i);
    }
}

void AimbotFPS::TickFrame(u32 buffer_index) {
    boxes.clear();
    auto& frame_buffer = frame_buffers[buffer_index];
    ProcessFrameData(frame_buffer.get());
    const u32 core_index = NextCoreIndex();
    models[core_index]->Detect(boxes, frame_buffer->out_data);
    for (const auto& box : boxes) {
        LOG_INFO("box: class_id: {}, score: {}, x1: {}, y1: {}, x2: {}, y2: {}",
                 box.class_id,
                 box.score,
                 box.x1,
                 box.y1,
                 box.x2,
                 box.y2);
    }
}

void AimbotFPS::ProcessFrameData(FrameBuffer* buffer) const {
    int ret = imcrop(buffer->in_img,
                     buffer->out_img,
                     {
                             .x = static_cast<int>((in_width - out_width) / 2),
                             .y = static_cast<int>((in_height - out_height) / 2),
                             .width = static_cast<int>(out_width),
                             .height = static_cast<int>(out_height),
                     });
    if (ret < 0) {
        throw std::runtime_error("imcrop failed");
    }
}

FrameBuffer::~FrameBuffer() {
    if (in_handle) {
        releasebuffer_handle(in_handle);
    }
    if (out_handle) {
        releasebuffer_handle(out_handle);
    }
}

}  // namespace aibox::aimbot
