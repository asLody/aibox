#include "core/aimbot/aimbot_fps.h"
#include "core/core.h"
#include "core/usb/mouse/proxy_mouse.h"
#include "core/usb/usb_gadget.h"

namespace aibox {

Core::Core() {
    gadget = std::make_unique<usb::UsbGadget>();
    aimbot = std::make_unique<aimbot::AimbotFPS>();
}

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
    aimbot->LoadModel("/home/firefly/cs2_RK3588_i8.rknn");
    aimbot->SetMoveCallback([this](float x, float y) {
        mouse->Send({
                .x = static_cast<double>(x),
                .y = static_cast<double>(y),
        });
    });
    aimbot->Run();
}

}  // namespace aibox