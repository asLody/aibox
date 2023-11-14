#pragma once

#include <memory>
#include <usbg/usbg.h>
#include "core/hid/mouse.h"

namespace aibox::hid {

class OTGDaemon {
public:
    OTGDaemon();

    ~OTGDaemon();

    void Start();

    void Stop() const;

    Mouse* GetMouse() const { return mouse.get(); }

private:
    std::shared_ptr<Mouse> mouse;
    usbg_state* u_state{};
    usbg_gadget* u_gadget{};
};

}  // namespace aibox::hid