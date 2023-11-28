#pragma once

#include <vector>
#include <libusb.h>
#include "base/types.h"
#include "core/usb/hid_device_descriptor.h"

namespace aibox::usb {

class InputHIDDevice {
public:
    InputHIDDevice();

    InputHIDDevice(u16 vid, u16 pid);

    virtual ~InputHIDDevice();

    void Open();

    [[nodiscard]] const HIDDeviceDescriptor& GetDescriptor() const { return descriptor; }

protected:
    virtual bool MatchInterfaceDescriptor(const libusb_interface_descriptor* interface_desc);

private:
    u16 vid, pid;
    HIDDeviceDescriptor descriptor{};
    u8 interface_num{};
    libusb_device_handle* handle{};

    void OpenUSB(libusb_device* dev,
                 const libusb_device_descriptor* desc,
                 const libusb_interface_descriptor* interface_desc);

    static size_t ParseReportDescriptorSize(const libusb_interface_descriptor* interface_desc);
};

}  // namespace aibox::usb
