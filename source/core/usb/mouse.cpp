#include <stdexcept>
#include <utility>
#include <fcntl.h>
#include <unistd.h>
#include "core/usb/mouse.h"

namespace aibox::usb {

Mouse::Mouse(MouseConfig config) : config(std::move(config)) {}

Mouse::~Mouse() { Close(); }

void Mouse::Open(const std::string& dev) {
    hid_fd = open(dev.c_str(), O_RDWR, 0666);
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

void Mouse::MoveTo(int16_t x, int16_t y) {
    cur_x = x;
    cur_y = y;
    SendReport();
}

bool Mouse::SendReport() {
    if (hid_fd < 0) {
        throw std::runtime_error("HID device not opened");
    }
    MouseReport report = {
            .buttons = pressed_buttons,
            .x = cur_x,
            .y = cur_y,
            .wheel_y = cur_wheel_y,
    };
    return write(hid_fd, &report, sizeof(report)) == sizeof(report);
}

}  // namespace aibox::usb
