#include <stdexcept>
#include <fcntl.h>
#include <unistd.h>
#include "core/usb/output_device.h"

namespace aibox::usb {

OutputDevice::~OutputDevice() { Close(); }

void OutputDevice::Open(const std::string& dev) {
    Close();
    fd = open(dev.c_str(), O_RDWR, 0666);
    if (fd < 0) {
        throw std::runtime_error("Error opening HID device");
    }
}

void OutputDevice::Close() {
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
}

ssize_t OutputDevice::Write(const void* data, size_t size) { return write(fd, data, size); }

}  // namespace aibox::usb