
#pragma once

#include <vector>
#include "base/types.h"

namespace aibox::video {

struct V4l2Buffer {
    u32 index;
    u8* buffer;
    u32 length;
};

class VideoCapture {
public:
    VideoCapture();

    void Open(s32 video_device_id = -1);

    const V4l2Buffer& DequeueBuffer();

    void RequeueBuffer(u32 index) const;

    void StartStreaming() const;

    void StopStreaming() const;

    [[nodiscard]] u32 GetFormat() const { return format; }

    [[nodiscard]] u32 GetWidth() const { return width; }

    [[nodiscard]] u32 GetHeight() const { return height; }

private:
    u32 buffer_count{3};
    int fd{-1};
    u32 format{};
    u32 width{}, height{};
    std::vector<V4l2Buffer> buffers;

    static std::string StrPixelFormat(u32 pixel_format);

    void WriteEDID(const u8* data, u32 len) const;

    void Setup();

    void MapBuffers();

    void QueueBuffers() const;
};

}  // namespace aibox::video