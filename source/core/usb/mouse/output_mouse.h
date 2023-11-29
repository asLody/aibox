#pragma once

#include "core/usb/mouse/mouse.h"

namespace aibox::usb {

class OutputMouse : public OutputHIDDevice {
public:
    explicit OutputMouse();

    ~OutputMouse() override;
};

}  // namespace aibox::usb