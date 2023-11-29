#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "base/types.h"
#include "core/usb/hid-parser/descriptor.h"

namespace aibox::usb {

struct HIDGadgetDescriptor {
    u16 bcd_usb;
    u8 device_subclass;
    u8 device_protocol;
    u8 max_packet_size0;

    u16 vendor_id;
    u16 product_id;
    u16 bcd_device;

    std::string manufacturer;
    std::string product;
    std::string serial;

    u32 hid_protocol;
    u32 hid_subclass;
    hid::DeviceDescriptor* report_descriptor;
    std::vector<u8> report_descriptor_data;
    u32 input_report_index;
    u16 report_length;

    ~HIDGadgetDescriptor();
};

}  // namespace aibox::usb