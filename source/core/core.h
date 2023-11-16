#pragma once

#include <memory>

namespace aibox {

namespace usb {
class UsbGadget;
class Mouse;
}  // namespace usb

class Core {
public:
    Core();

    ~Core();

    void StartSystem();

    usb::UsbGadget *GetUsbGadget();

private:
    std::unique_ptr<usb::UsbGadget> gadget;
};

}  // namespace aibox