#include <unistd.h>
#include <stdexcept>
#include <thread>
#include "core/core.h"

int main(int argc, char** argv) {
    if (geteuid() != 0) {
        throw std::runtime_error("You must run this program as root.");
    }
    aibox::Core core;
    core.Run();
    for (;;) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}