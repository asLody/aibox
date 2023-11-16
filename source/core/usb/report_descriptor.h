#pragma once

#include <cstdint>
#include <vector>

namespace aibox::usb {

class ReportDescriptor {
public:
    [[nodiscard]] std::vector<char> GetBytes() const;

    [[nodiscard]] uint32_t GetReportLength() const;
};

}  // namespace aibox::usb