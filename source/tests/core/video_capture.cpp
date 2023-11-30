#include <catch2/catch_test_macros.hpp>
#include "core/video/video_capture.h"

TEST_CASE("Video::VideoCapture", "[core]") {
    aibox::video::VideoCapture video_capture;
    video_capture.Open();
    video_capture.StartStreaming();
    const auto& buffer = video_capture.DequeueBuffer();
    video_capture.RequeueBuffer(buffer.index);
    video_capture.StopStreaming();
}
