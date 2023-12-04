#include "base/file_utils.h"
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
    protocol.ParseReportDescriptor(descriptor.report_descriptor);
    descriptor.report_length = protocol.GetHIDReportLength();
}

}  // namespace aibox::usb
