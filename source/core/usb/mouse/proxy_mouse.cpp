#include "core/usb/mouse/proxy_mouse.h"

namespace aibox::usb {

ProxyMouse::ProxyMouse(HIDDeviceIdentifier identifier) : InputMouse(identifier) {}

ProxyMouse::~ProxyMouse() = default;

void ProxyMouse::InitProtocol() {
    InputMouse::InitProtocol();
    buffer.resize(descriptor.report_length);
}

void ProxyMouse::ConnectOutput(u32 hid_number) { output.Open(hid_number); }

void ProxyMouse::HandleInput(u8* data, u32 length) { output.Write(data, length); }

void ProxyMouse::Send(const MouseReport& report) {
    protocol.Encode(buffer, report);
    output.Write(buffer.data(), buffer.size());
}

}  // namespace aibox::usb
