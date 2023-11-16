#pragma once

#include <string>
#include <vector>
#include "core/usb/config.h"
#include "core/usb/report_descriptor.h"

namespace aibox::usb {

struct __attribute__((packed)) MouseReport {
    uint8_t buttons;
    int16_t x;
    int16_t y;
    int8_t wheel_y;
};

class Mouse {
public:
    explicit Mouse(MouseConfig config);

    ~Mouse();

    void Open(const std::string& dev);

    void Close();

    void MoveTo(int16_t x, int16_t y);

    bool SendReport();

private:
    MouseConfig config;
    int hid_fd = -1;
    uint8_t pressed_buttons{};
    int16_t cur_x{}, cur_y{};
    int8_t cur_wheel_y{};
};

}  // namespace aibox::usb