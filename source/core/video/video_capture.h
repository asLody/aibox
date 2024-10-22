
#pragma once

#include <vector>
#include "base/types.h"

namespace aibox::video {

struct VideoBuffer {
    u32 index;
    u8* buffer;
    u32 length;
};

class VideoCapture {
public:
    VideoCapture();

    ~VideoCapture();

    void Open(s32 video_device_id = -1);

    void Close();

    u32 DequeueBuffer() const;

    void RequeueBuffer(u32 index) const;

    void StartStreaming() const;

    void StopStreaming() const;

    [[nodiscard]] u32 GetFormat() const { return format; }

    [[nodiscard]] u32 GetWidth() const { return width; }

    [[nodiscard]] u32 GetHeight() const { return height; }

    [[nodiscard]] const std::vector<VideoBuffer>& GetBuffers() const { return buffers; }

private:
    u32 buffer_count{3};
    int fd{-1};
    u32 format{};
    u32 width{}, height{};
    std::vector<VideoBuffer> buffers;

    static std::string StrPixelFormat(u32 pixel_format);

    void WriteEDID(const u8* data, u32 len) const;

    void Setup();

    void MapBuffers();

    void QueueBuffers() const;
};

}  // namespace aibox::video