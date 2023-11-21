#include <thread>
#include "core/core.h"
#include "core/usb/mouse.h"
#include "core/usb/usb_gadget.h"

int main(int argc, char** argv) {
    aibox::Core core;
    core.StartSystem();
    auto mouse = core.GetUsbGadget()->GetMouse();
    for (;;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        mouse->MoveBy(10, 0);
    }
    return 0;
}