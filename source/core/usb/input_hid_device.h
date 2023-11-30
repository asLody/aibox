#pragma once

#include <thread>
#include <vector>
#include <libusb.h>
#include "base/types.h"
#include "core/usb/hid_device_identifier.h"
#include "core/usb/hid_gadget_descriptor.h"

namespace aibox::usb {

class InputHIDDevice {
public:
    explicit InputHIDDevice(HIDDeviceIdentifier identifier);

    virtual ~InputHIDDevice();

    void Open();

    void StartReadInput();

    void StopReadInput();

    [[nodiscard]] const HIDGadgetDescriptor& GetDescriptor() const { return descriptor; }

protected:
    const HIDDeviceIdentifier identifier;
    HIDGadgetDescriptor descriptor{};

    virtual bool MatchInterfaceDescriptor(const libusb_interface_descriptor* interface_desc);

    virtual void InitProtocol() {}

    virtual void HandleInput(u8* data, u32 length) {}

private:
    u8 interface_num{};
    libusb_context* usb_ctx{};
    libusb_device_handle* usb_handle{};
    libusb_transfer* usb_transfer{};
    u16 input_ep_max_packet_size{};
    u8 input_endpoint{};
    u8 output_endpoint{};
    bool is_running{};
    std::thread thread;

    void OpenUSB(libusb_device* dev,
                 const libusb_device_descriptor* desc,
                 const libusb_interface_descriptor* interface_desc);

    void ReadCallback(libusb_transfer* transfer);

    static size_t ParseReportDescriptorSize(const libusb_interface_descriptor* interface_desc);
};

}  // namespace aibox::usb
