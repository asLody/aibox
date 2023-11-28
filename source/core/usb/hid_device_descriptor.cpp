#include "core/usb/hid_device_descriptor.h"

namespace aibox::usb {

HIDDeviceDescriptor::~HIDDeviceDescriptor() { hid::FreeDeviceDescriptor(report_descriptor); }

}  // namespace aibox::usb
