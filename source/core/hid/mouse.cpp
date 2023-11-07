#include <unistd.h>
#include <fcntl.h>
#include "core/hid/mouse.h"

namespace aibox::hid {

MouseConfig Mouse::GetDefaultConfig() {
    return {
            .vendor_id = 0x1D6B,
            .product_id = 0x0104,
            .manufacturer = "AIBOX",
            .product = "AIBOX Mouse",
            .serial = "CAFEBABE",
            .configuration = "1xMouse",
            .max_power = 250,
            .device_version = 0x200,
    };
}


void Mouse::Open(const std::string &dev) {
    hid_fd = open(dev.c_str(), O_RDWR);
    if (hid_fd < 0) {
        throw std::runtime_error("Error opening HID device");
    }
}

void Mouse::Close() {
    if (hid_fd >= 0) {
        close(hid_fd);
        hid_fd = -1;
    }
}


std::vector<char> Mouse::GetReportDescriptor() const {
    return {
            // Absolute mouse
            0x05, 0x01,  // USAGE_PAGE (Generic Desktop)
            0x09, 0x02,  // USAGE (Mouse)
            0xA1, 0x01,  // COLLECTION (Application)

            // Pointer and Physical are required by Apple Recovery
            0x09, 0x01,  // USAGE (Pointer)
            0xA1, 0x00,  // COLLECTION (Physical)

            // 8 Buttons
            0x05, 0x09,  // USAGE_PAGE (Button)
            0x19, 0x01,  // USAGE_MINIMUM (Button 1)
            0x29, 0x08,  // USAGE_MAXIMUM (Button 8)
            0x15, 0x00,  // LOGICAL_MINIMUM (0)
            0x25, 0x01,  // LOGICAL_MAXIMUM (1)
            0x95, 0x08,  // REPORT_COUNT (8)
            0x75, 0x01,  // REPORT_SIZE (1)
            0x81, 0x02,  // INPUT (Data,Var,Abs)

            // X, Y
            0x05, 0x01,  // USAGE_PAGE (Generic Desktop)
            0x09, 0x30,  // USAGE (X)
            0x09, 0x31,  // USAGE (Y)
            0x16, 0x00, 0x00,  // LOGICAL_MINIMUM (0)
            0x26, 0xFF, 0x7F,  // LOGICAL_MAXIMUM (32767)
            0x75, 0x10,  // REPORT_SIZE (16)
            0x95, 0x02,  // REPORT_COUNT (2)
            0x81, 0x02,  // INPUT (Data,Var,Abs)

            // Wheel
            0x09, 0x38,  // USAGE (Wheel)
            0x15, 0x81,  // LOGICAL_MINIMUM (-127)
            0x25, 0x7F,  // LOGICAL_MAXIMUM (127)
            0x75, 0x08,  // REPORT_SIZE (8)
            0x95, 0x01,  // REPORT_COUNT (1)
            0x81, 0x06,  // INPUT (Data,Var,Rel)

            // End
            0xC0,  // END_COLLECTION (Physical)
            0xC0,  // END_COLLECTION
    };
}

void Mouse::MoveTo(int16_t x, int16_t y) {
    cur_x = x;
    cur_y = y;
    SendReport();
}

void Mouse::MoveWheel(int8_t y) {
    wheel_y = y;
    SendReport();
}

bool Mouse::SendReport() {
    if (hid_fd < 0) {
        return false;
    }
    MouseReport report = {
            .buttons = pressed_buttons,
            .x = cur_x,
            .y = cur_y,
            .wheel = wheel_y,
    };
    return write(hid_fd, &report, sizeof(report)) == sizeof(report);
}

Mouse::~Mouse() {
    Close();
}

}
