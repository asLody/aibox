#include <catch2/catch_test_macros.hpp>
#include "core/video/video_capture.h"

TEST_CASE("Video::VideoCapture", "[core]") {
    aibox::video::VideoCapture video_capture;
    video_capture.Open();
    video_capture.StartStreaming();
    const auto &buffers = video_capture.GetBuffers();
    REQUIRE(buffers.size() == 3);
    const auto index = video_capture.DequeueBuffer();
    video_capture.RequeueBuffer(index);
    video_capture.StopStreaming();
}
