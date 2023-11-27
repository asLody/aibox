#pragma once

#include <string>
#include <vector>
#include "base/macros.h"
#include "core/usb/config.h"
#include "core/usb/output_device.h"
#include "core/usb/report_descriptor.h"

namespace aibox::usb {

struct __attribute__((packed)) MouseReport {
    uint8_t buttons;
    int8_t x;
    int8_t y;
    int8_t wheel_y;
};

class Mouse : public OutputDevice {
public:
    explicit Mouse(MouseConfig config);

    ~Mouse();

    void MoveBy(int8_t x, int8_t y);

    ALWAYS_INLINE bool SendReport(const MouseReport& report);

private:
    MouseConfig config;
};

}  // namespace aibox::usb