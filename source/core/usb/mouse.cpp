#include <stdexcept>
#include <utility>
#include "core/usb/mouse.h"

namespace aibox::usb {

Mouse::Mouse(MouseConfig config) : config(std::move(config)) {}

Mouse::~Mouse() { Close(); }

void Mouse::MoveBy(int8_t x, int8_t y) {
    if (!SendReport({.x = x, .y = y})) {
        throw std::runtime_error("Error sending mouse report");
    }
}

bool Mouse::SendReport(const MouseReport& report) {
    return Write(&report, sizeof(report)) == sizeof(report);
}

}  // namespace aibox::usb
