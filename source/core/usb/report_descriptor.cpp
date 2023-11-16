#include "core/usb/report_descriptor.h"

namespace aibox::usb {

std::vector<char> ReportDescriptor::GetBytes() const {
    return {
            // Absolute mouse
            0x05,
            0x01,  // USAGE_PAGE (Generic Desktop)
            0x09,
            0x02,  // USAGE (Mouse)
            0xA1,
            0x01,  // COLLECTION (Application)

            // Pointer and Physical are required by Apple Recovery
            0x09,
            0x01,  // USAGE (Pointer)
            0xA1,
            0x00,  // COLLECTION (Physical)

            // 8 Buttons
            0x05,
            0x09,  // USAGE_PAGE (Button)
            0x19,
            0x01,  // USAGE_MINIMUM (Button 1)
            0x29,
            0x08,  // USAGE_MAXIMUM (Button 8)
            0x15,
            0x00,  // LOGICAL_MINIMUM (0)
            0x25,
            0x01,  // LOGICAL_MAXIMUM (1)
            0x95,
            0x08,  // REPORT_COUNT (8)
            0x75,
            0x01,  // REPORT_SIZE (1)
            0x81,
            0x02,  // INPUT (Data,Var,Abs)

            // X, Y
            0x05,
            0x01,  // USAGE_PAGE (Generic Desktop)
            0x09,
            0x30,  // USAGE (X)
            0x09,
            0x31,  // USAGE (Y)
            0x16,
            0x00,
            0x00,  // LOGICAL_MINIMUM (0)
            0x26,
            0xFF,
            0x7F,  // LOGICAL_MAXIMUM (32767)
            0x75,
            0x10,  // REPORT_SIZE (16)
            0x95,
            0x02,  // REPORT_COUNT (2)
            0x81,
            0x02,  // INPUT (Data,Var,Abs)

            // Wheel
            0x09,
            0x38,  // USAGE (Wheel)
            0x15,
            0x81,  // LOGICAL_MINIMUM (-127)
            0x25,
            0x7F,  // LOGICAL_MAXIMUM (127)
            0x75,
            0x08,  // REPORT_SIZE (8)
            0x95,
            0x01,  // REPORT_COUNT (1)
            0x81,
            0x06,  // INPUT (Data,Var,Rel)

            // End
            0xC0,  // END_COLLECTION (Physical)
            0xC0,  // END_COLLECTION
    };
}

uint32_t ReportDescriptor::GetReportLength() const { return 6; }

}  // namespace aibox::usb