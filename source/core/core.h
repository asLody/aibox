#pragma once

#include <memory>

namespace aibox {

namespace hid {
class OTGDaemon;
class Mouse;
}  // namespace hid

class Core {
public:
    Core();

    ~Core();

    void StartSystem();

    hid::Mouse* GetMouse() const;

private:
    std::unique_ptr<hid::OTGDaemon> otg_daemon;
};

}  // namespace aibox