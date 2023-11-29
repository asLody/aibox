#include <stdexcept>
#include <fcntl.h>
#include <unistd.h>
#include "core/usb/output_hid_device.h"
#include "fmt/format.h"

namespace aibox::usb {

OutputHIDDevice::~OutputHIDDevice() { Close(); }

void OutputHIDDevice::Open(u32 hid_number) {
    Close();
    const auto device_path = fmt::format("/dev/hidg{}", hid_number);
    fd = open(device_path.c_str(), O_RDWR, 0666);
    if (fd < 0) {
        throw std::runtime_error("Error opening HID device");
    }
}

void OutputHIDDevice::Close() {
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
}

ssize_t OutputHIDDevice::Write(const void* data, size_t size) const {
    if (IsOpened()) {
        return write(fd, data, size);
    }
    return -1;
}

}  // namespace aibox::usb