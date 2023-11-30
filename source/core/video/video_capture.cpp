#include <dirent.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include "base/alignment.h"
#include "base/logging.h"
#include "core/video/edid.h"
#include "core/video/video_capture.h"

namespace aibox::video {

std::string VideoCapture::StrPixelFormat(u32 pixel_format) {
    std::array<char, 5> fourcc{};
    strncpy(fourcc.data(), reinterpret_cast<char*>(&pixel_format), 4);
    return fourcc.data();
}

VideoCapture::VideoCapture() = default;

void VideoCapture::Open(s32 video_device_id) {
    if (video_device_id >= 0) {
        fd = open(fmt::format("/dev/video{}", video_device_id).c_str(), O_RDWR);
    } else {
        DIR* dir = opendir("/dev");
        if (dir == nullptr) {
            throw std::runtime_error("Could not open /dev");
        }
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (strncmp(entry->d_name, "video", 5) == 0 && isdigit(entry->d_name[5])) {
                fd = open(fmt::format("/dev/{}", entry->d_name).c_str(), O_RDWR);
                if (fd >= 0) {
                    break;
                }
            }
        }
        closedir(dir);
    }
    if (fd < 0) {
        throw std::runtime_error("Could not open video device");
    }
    Setup();
}

void VideoCapture::Setup() {
    v4l2_capability capability{};
    if (ioctl(fd, VIDIOC_QUERYCAP, &capability) < 0) {
        throw std::runtime_error("Could not query video device capabilities");
    }
    if ((capability.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE) == 0) {
        throw std::runtime_error("Video device does not support multi-planar API");
    }
    if ((capability.capabilities & V4L2_CAP_STREAMING) == 0) {
        throw std::runtime_error("Video device does not support streaming");
    }
    v4l2_format get_format_command{
            .type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE,
    };
    if (ioctl(fd, VIDIOC_G_FMT, &get_format_command) < 0) {
        throw std::runtime_error("Could not query video device format");
    }
    if (get_format_command.fmt.pix_mp.pixelformat != V4L2_PIX_FMT_BGR24) {
        LOG_DEBUG("Current pixel format is not BGR24 (got {}), writing edid...",
                  StrPixelFormat(get_format_command.fmt.pix_mp.pixelformat));
        WriteEDID(EDID_FORCE_GBR24, sizeof(EDID_FORCE_GBR24));
    }
    if (ioctl(fd, VIDIOC_G_FMT, &get_format_command) < 0) {
        throw std::runtime_error("Could not query video device format");
    }
    if (get_format_command.fmt.pix_mp.pixelformat != V4L2_PIX_FMT_BGR24) {
        throw std::runtime_error(
                fmt::format("Video device does not support RGB24 format (got {})",
                            StrPixelFormat(get_format_command.fmt.pix_mp.pixelformat)));
    }
    format = get_format_command.fmt.pix_mp.pixelformat;
    width = get_format_command.fmt.pix_mp.width;
    height = get_format_command.fmt.pix_mp.height;
    LOG_INFO("Video device format: {}x{} {}",
             width,
             height,
             StrPixelFormat(get_format_command.fmt.pix_mp.pixelformat));
    MapBuffers();
}

void VideoCapture::MapBuffers() {
    v4l2_requestbuffers request_buffers{
            .count = buffer_count,
            .type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE,
            .memory = V4L2_MEMORY_MMAP,
    };
    if (ioctl(fd, VIDIOC_REQBUFS, &request_buffers) < 0) {
        throw std::runtime_error("Could not request buffers");
    }
    if (request_buffers.count < buffer_count) {
        throw std::runtime_error("Could not allocate enough buffers");
    }
    buffers.resize(request_buffers.count);
    for (u32 i = 0; i < request_buffers.count; ++i) {
        v4l2_plane planes[VIDEO_MAX_PLANES]{};
        v4l2_buffer buffer{
                .index = i,
                .type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE,
                .memory = V4L2_MEMORY_MMAP,
                .length = VIDEO_MAX_PLANES,
        };
        buffer.m.planes = planes;

        if (ioctl(fd, VIDIOC_QUERYBUF, &buffer) < 0) {
            throw std::runtime_error("Could not query buffer");
        }
        buffers[i].index = i;
        buffers[i].length = buffer.m.planes[0].length;
        buffers[i].buffer = static_cast<u8*>(mmap(nullptr,
                                                  buffer.m.planes[0].length,
                                                  PROT_READ | PROT_WRITE,
                                                  MAP_SHARED,
                                                  fd,
                                                  buffer.m.planes[0].m.mem_offset));
        if (buffers[i].buffer == MAP_FAILED) {
            throw std::runtime_error("Could not map buffer");
        }
    }
}

void VideoCapture::QueueBuffers() const {
    for (u32 i = 0; i < buffer_count; ++i) {
        v4l2_plane planes[VIDEO_MAX_PLANES]{};
        v4l2_buffer buffer{
                .index = i,
                .type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE,
                .memory = V4L2_MEMORY_MMAP,
                .length = VIDEO_MAX_PLANES,
        };
        buffer.m.planes = planes;
        if (ioctl(fd, VIDIOC_QBUF, &buffer) < 0) {
            throw std::runtime_error("Could not queue buffer");
        }
    }
}

u32 VideoCapture::DequeueBuffer() const {
    v4l2_plane planes[VIDEO_MAX_PLANES]{};
    v4l2_buffer buffer{
            .type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE,
            .memory = V4L2_MEMORY_MMAP,
            .length = VIDEO_MAX_PLANES,
    };
    buffer.m.planes = planes;
    if (ioctl(fd, VIDIOC_DQBUF, &buffer) < 0) {
        throw std::runtime_error("Could not dequeue buffer");
    }
    return buffer.index;
}

void VideoCapture::RequeueBuffer(u32 index) const {
    v4l2_plane planes[VIDEO_MAX_PLANES]{};
    v4l2_buffer buffer{
            .index = index,
            .type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE,
            .memory = V4L2_MEMORY_MMAP,
            .length = VIDEO_MAX_PLANES,
    };
    buffer.m.planes = planes;
    if (ioctl(fd, VIDIOC_QBUF, &buffer) < 0) {
        throw std::runtime_error("Could not queue buffer");
    }
}

void VideoCapture::StartStreaming() const {
    QueueBuffers();
    u32 type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        throw std::runtime_error("Could not start streaming");
    }
}

void VideoCapture::StopStreaming() const {
    u32 type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if (ioctl(fd, VIDIOC_STREAMOFF, &type) < 0) {
        throw std::runtime_error("Could not stop streaming");
    }
}
void VideoCapture::WriteEDID(const u8* data, u32 len) const {
    const u32 blocks = base::DivUp(len, EDID_BLOCK_SIZE);
    std::vector<u8> aligned_data(EDID_BLOCK_SIZE * blocks);
    memcpy(aligned_data.data(), data, len);
    v4l2_edid edid{.start_block = 0, .blocks = blocks, .edid = aligned_data.data()};
    if (ioctl(fd, VIDIOC_S_EDID, &edid) < 0) {
        throw std::runtime_error("Could not set EDID");
    }
}

}  // namespace aibox::video