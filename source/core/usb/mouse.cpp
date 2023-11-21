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

void Mouse::MoveBy(int8_t x, int8_t y) {
    if (!SendReport({.x = x, .y = y})) {
        throw std::runtime_error("Error sending mouse report");
    }
}

bool Mouse::SendReport(const MouseReport& report) {
    if (hid_fd < 0) {
        throw std::runtime_error("HID device not opened");
    }
    return write(hid_fd, &report, sizeof(report)) == sizeof(report);
}

}  // namespace aibox::usb
