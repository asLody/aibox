#pragma once

#include <memory>
#include <usbg/usbg.h>
#include "core/usb/hid_device_identifer.h"
#include "core/usb/hid_gadget_descriptor.h"

namespace aibox::usb {

class UsbGadget {
public:
    UsbGadget();

    ~UsbGadget();

    [[nodiscard]] HIDDeviceIdentifier GetConfiguredIdentifier() const;

    void Configure(const HIDGadgetDescriptor& descriptor);

    void RemoveGadgets();

private:
    const char* gadget_name = "aibox";
    const char* default_serial = "0";
    usbg_state* u_state{};
    usbg_gadget* u_gadget{};

    void CreateGadget(const HIDGadgetDescriptor& descriptor);

    void CheckExistingGadget(const HIDGadgetDescriptor& descriptor);

    static void PrintDescriptor(const HIDGadgetDescriptor& descriptor);
};

}  // namespace aibox::usb