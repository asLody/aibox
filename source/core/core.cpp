#include "core/core.h"
#include "core/usb/mouse/proxy_mouse.h"
#include "core/usb/usb_gadget.h"

namespace aibox {

Core::Core() { gadget = std::make_unique<usb::UsbGadget>(); }

Core::~Core() = default;

void Core::Setup() {
    const auto identifier = gadget->GetConfiguredIdentifier();
    mouse = std::make_unique<usb::ProxyMouse>(identifier);
}

void Core::Run() {
    if (is_started) {
        throw std::runtime_error("System already started");
    }
    is_started = true;
    Setup();
    mouse->Open();
    gadget->Configure(mouse->GetDescriptor());
    mouse->ConnectOutput(0);
    mouse->StartReadInput();
}

}  // namespace aibox