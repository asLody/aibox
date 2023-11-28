#include <stdexcept>
#include <fcntl.h>
#include <unistd.h>
#include "core/usb/output_hid_device.h"

namespace aibox::usb {

OutputHIDDevice::~OutputHIDDevice() { Close(); }

void OutputHIDDevice::Open(const std::string& dev) {
    Close();
    fd = open(dev.c_str(), O_RDWR, 0666);
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

ssize_t OutputHIDDevice::Write(const void* data, size_t size) { return write(fd, data, size); }

}  // namespace aibox::usb