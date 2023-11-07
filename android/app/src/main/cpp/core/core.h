#pragma once

#include <memory>

namespace aibox {

namespace hid {
class OTGDaemon;
class Mouse;
}

class Core {
public:
    Core();

    void StartSystem();

    hid::Mouse *GetMouse() const;

private:
    std::unique_ptr<hid::OTGDaemon> otg_daemon;
};

}