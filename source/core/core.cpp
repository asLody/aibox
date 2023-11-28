#include "core/core.h"
#include "core/usb/usb_gadget.h"

namespace aibox {

Core::Core() { gadget = std::make_unique<usb::UsbGadget>(); }

void Core::StartSystem() {

}

usb::UsbGadget* Core::GetUsbGadget() { return gadget.get(); }

Core::~Core() = default;

}  // namespace aibox