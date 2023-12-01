#pragma once

#include <memory>

namespace aibox {

namespace usb {
class UsbGadget;
class ProxyMouse;
}  // namespace usb

namespace aimbot {
class AimbotFPS;
}  // namespace aimbot

class Core {
public:
    Core();

    ~Core();

    void Run();

    usb::UsbGadget* GetUsbGadget() const { return gadget.get(); }

    usb::ProxyMouse* GetProxyMouse() const { return mouse.get(); }

private:
    bool is_started = false;
    std::unique_ptr<usb::UsbGadget> gadget;
    std::unique_ptr<usb::ProxyMouse> mouse;
    std::unique_ptr<aimbot::AimbotFPS> aimbot;

    void Setup();
};

}  // namespace aibox