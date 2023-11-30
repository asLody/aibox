#pragma once

#include "base/types.h"

namespace aibox::usb {

struct HIDDeviceIdentifier {
    u16 vid{};
    u16 pid{};
};

}  // namespace aibox::usb