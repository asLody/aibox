#include "core/usb/config.h"

namespace aibox::usb {

MouseConfig MouseConfig::GetDefault() {
    return {
            .vendor_id = 0x1D6B,
            .product_id = 0x0104,
            .manufacturer = "AIBOX",
            .product = "AIBOX Mouse",
            .serial = "CAFEBABE",
            .configuration = "Usb Mouse",
            .max_power = 250,
            .device_version = 0x201,
            .report_descriptor = {},
    };
}

}  // namespace aibox::usb
