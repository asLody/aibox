#include <thread>
#include "core/core.h"
#include "core/hid/mouse.h"

int main(int argc, char** argv) {
    aibox::Core core;
    core.StartSystem();
    for (;;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return 0;
}