#pragma once

#include <cstdint>
#include <string>
#include "core/usb/report_descriptor.h"

namespace aibox::usb {

struct MouseConfig {
    uint16_t vendor_id;
    uint16_t product_id;
    std::string manufacturer;
    std::string product;
    std::string serial;
    std::string configuration;
    uint32_t max_power;
    uint16_t device_version;

    ReportDescriptor report_descriptor;

    static MouseConfig GetDefault();
};

}  // namespace aibox::usb