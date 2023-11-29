#pragma once

#include <memory>

namespace aibox {

namespace usb {
class UsbGadget;
class ProxyMouse;
}  // namespace usb

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

    void Setup();
};

}  // namespace aibox