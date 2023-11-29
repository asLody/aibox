#include "core/usb/mouse/input_mouse.h"

namespace aibox::usb {

InputMouse::InputMouse(HIDDeviceIdentifier identifier) : InputHIDDevice(identifier) {}

InputMouse::~InputMouse() = default;

bool InputMouse::MatchInterfaceDescriptor(const libusb_interface_descriptor* interface_desc) {
    if (!InputHIDDevice::MatchInterfaceDescriptor(interface_desc)) {
        return false;
    }
    return interface_desc->bInterfaceProtocol == 2 /* mouse */;
}

void InputMouse::InitProtocol() {
    const auto& desc = descriptor.report_descriptor;
    protocol.ParseReportDescriptor(desc->report[descriptor.input_report_index]);
}

}  // namespace aibox::usb
