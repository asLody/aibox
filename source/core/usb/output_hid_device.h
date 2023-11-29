#pragma once

#include <string>
#include "base/types.h"

namespace aibox::usb {

class OutputHIDDevice {
public:
    virtual ~OutputHIDDevice();

    void Open(u32 hid_number);

    void Close();

    ssize_t Write(const void* data, size_t size) const;

    [[nodiscard]] bool IsOpened() const { return fd >= 0; }

private:
    s32 fd{-1};
};

}  // namespace aibox::usb