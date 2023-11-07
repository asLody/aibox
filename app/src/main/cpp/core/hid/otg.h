#pragma once

#include <memory>
#include "core/hid/mouse.h"

namespace aibox::hid {

class OTGDaemon {
public:
    OTGDaemon();

private:
    std::shared_ptr<Mouse> mouse;
    usbg_state *u_state{};

    void Start();

    void Stop();
};

}