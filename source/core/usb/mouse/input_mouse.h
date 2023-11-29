#pragma once

#include "core/usb/input_hid_device.h"
#include "core/usb/mouse/mouse.h"

namespace aibox::usb {

class InputMouse : public InputHIDDevice {
public:
    explicit InputMouse(HIDDeviceIdentifier identifier);

    ~InputMouse() override;

    [[nodiscard]] const MouseProtocol& GetProtocol() const { return protocol; }

protected:
    MouseProtocol protocol;

    bool MatchInterfaceDescriptor(const libusb_interface_descriptor* interface_desc) override;

    void InitProtocol() override;
};

}  // namespace aibox::usb