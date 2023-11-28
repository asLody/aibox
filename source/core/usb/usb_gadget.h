#pragma once

#include <memory>
#include <usbg/usbg.h>
#include "core/usb/hid_device_descriptor.h"

namespace aibox::usb {

class UsbGadget {
public:
    UsbGadget();

    ~UsbGadget();

    void Configure(const HIDDeviceDescriptor& descriptor);

    void RemoveGadgets();

private:
    const char* gadget_name = "aibox";
    usbg_state* u_state{};
    usbg_gadget* u_gadget{};

    void CreateGadget(const HIDDeviceDescriptor& descriptor);
};

}  // namespace aibox::usb