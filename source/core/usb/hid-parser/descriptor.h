#pragma once

#include <cstdint>
#include "core/usb/hid-parser/parser.h"

namespace hid {

enum ReportType : uint32_t {
    kReportInput = 1,
    kReportOutput = 2,
    kReportFeature = 3,
};

// Gets the size of the report from the first byte of the report.
size_t GetReportSizeFromFirstByte(const DeviceDescriptor& desc, ReportType type, uint8_t byte);

}  // namespace hid
