#pragma once

#include <string>

namespace aibox::usb {

class OutputDevice {
public:
    OutputDevice() = default;

    virtual ~OutputDevice();

    void Open(const std::string& dev);

    void Close();

    ssize_t Write(const void* data, size_t size);

private:
    int fd{-1};
};

}  // namespace aibox::usb