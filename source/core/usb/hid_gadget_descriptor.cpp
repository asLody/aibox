#include "core/usb/hid_gadget_descriptor.h"

namespace aibox::usb {

HIDGadgetDescriptor::~HIDGadgetDescriptor() {
    if (report_descriptor) {
        hid::FreeDeviceDescriptor(report_descriptor);
    }
}

}  // namespace aibox::usb
