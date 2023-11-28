#include <thread>
#include "core/core.h"
#include "core/usb/mouse.h"
#include "core/usb/usb_gadget.h"

int main(int argc, char** argv) {
    aibox::Core core;
    core.StartSystem();
    return 0;
}