#pragma once

#include <string>
#include <vector>
#include "base/macros.h"
#include "core/usb/config.h"
#include "core/usb/report_descriptor.h"

namespace aibox::usb {

struct __attribute__((packed)) MouseReport {
    uint8_t buttons;
    int8_t x;
    int8_t y;
    int8_t wheel_y;
};

class Mouse {
public:
    explicit Mouse(MouseConfig config);

    ~Mouse();

    void Open(const std::string& dev);

    void Close();

    void MoveBy(int8_t x, int8_t y);

    ALWAYS_INLINE bool SendReport(const MouseReport& report);

private:
    MouseConfig config;
    int hid_fd = -1;
    uint8_t pressed_buttons{};
};

}  // namespace aibox::usb