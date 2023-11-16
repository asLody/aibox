#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace aibox::usb {

class ReportDescriptor {
public:
    [[nodiscard]] std::vector<char> GetBytes() const;

    [[nodiscard]] uint32_t GetReportLength() const;

    void WriteToFile(const std::string& path) const;
};

}  // namespace aibox::usb