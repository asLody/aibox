#include <chrono>
#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include <im2d.hpp>
#include "base/file_utils.h"

TEST_CASE("RGA::Copy", "[external]") {
    const int src_width = 3840, src_height = 2160;
    const int dst_width = 640, dst_height = 640;

    const auto in_data = base::ReadFile("/home/firefly/4k_bgr3.raw");
    REQUIRE(!in_data.empty());
    std::vector<u8> out_data(dst_width * dst_height * 3);
    rga_buffer_handle_t src_handle =
            importbuffer_virtualaddr((void*)in_data.data(), (int)in_data.size());
    rga_buffer_handle_t dst_handle =
            importbuffer_virtualaddr((void*)out_data.data(), (int)out_data.size());

    rga_buffer_t src_img = wrapbuffer_handle(src_handle, src_width, src_height, RK_FORMAT_BGR_888);
    rga_buffer_t dst_img = wrapbuffer_handle(dst_handle, dst_width, dst_height, RK_FORMAT_RGB_888);
    int ret = imcheck(src_img, dst_img, {}, {});
    REQUIRE(ret == IM_STATUS_NOERROR);

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    for (int i = 0; i < 10; ++i) {
        imcrop(src_img,
               dst_img,
               {
                       .x = 0,
                       .y = 0,
                       .width = dst_width,
                       .height = dst_height,
               });
        REQUIRE(ret == IM_STATUS_NOERROR);
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "imcrop time = "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms"
              << std::endl;

    releasebuffer_handle(src_handle);
    releasebuffer_handle(dst_handle);

    REQUIRE(base::WriteFile("/home/firefly/crop_rgb3_out.raw", out_data.data(), out_data.size()));
}
