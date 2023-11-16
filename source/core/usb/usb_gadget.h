#pragma once

#include <memory>
#include <usbg/usbg.h>
#include "core/usb/mouse.h"
#include "core/usb/report_descriptor.h"

namespace aibox::usb {

class UsbGadget {
public:
    UsbGadget();

    ~UsbGadget();

    void Configure();

    void RemoveGadgets();

    Mouse* GetMouse() { return mouse.get(); }

private:
    const char* gadget_name = "aibox";
    usbg_state* u_state{};
    usbg_gadget* u_gadget{};

    MouseConfig config = MouseConfig::GetDefault();
    std::unique_ptr<Mouse> mouse;

    void CreateGadget();
};

}  // namespace aibox::usb