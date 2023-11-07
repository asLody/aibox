#include "core/core.h"
#include "core/hid/otg.h"

namespace aibox {


Core::Core() {
    otg_daemon = std::make_unique<hid::OTGDaemon>();
}

void Core::StartSystem() {
    otg_daemon->Start();
}

hid::Mouse *Core::GetMouse() const {
    return otg_daemon->GetMouse();
}

}