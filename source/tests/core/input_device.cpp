#include <catch2/catch_test_macros.hpp>
#include "core/usb/mouse.h"

TEST_CASE("USB::InputDevice", "[core]") {
    using namespace aibox::usb;
    InputMouse device(0x2a06, 0x4101);
    device.Open();
}
