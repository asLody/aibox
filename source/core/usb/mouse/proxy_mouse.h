#pragma once

#include "core/usb/mouse/input_mouse.h"
#include "core/usb/mouse/output_mouse.h"

namespace aibox::usb {

class ProxyMouse : public InputMouse {
public:
    explicit ProxyMouse(HIDDeviceIdentifier identifier);

    ~ProxyMouse() override;

    void ConnectOutput(u32 hid_number);

    void Send(const MouseReport& report);

protected:
    void InitProtocol() override;

    void HandleInput(u8* data, u32 length) override;

private:
    OutputMouse output;
    std::vector<u8> buffer;
};

}  // namespace aibox::usb