#include <catch2/catch_test_macros.hpp>
#include "core/usb/hid-parser/parser.h"
#include "core/usb/hid-parser/usages.h"
#include "core/usb/mouse.h"

const uint8_t test_mouse_desc[] = {
        /*  Mouse absolute */
        0x05,
        0x01, /* USAGE_PAGE (Generic Desktop)	  54 */
        0x09,
        0x02, /* USAGE (Mouse) */
        0xA1,
        0x01, /* COLLECTION (Application) */

        /* 8 Buttons */
        0x05,
        0x09, /*     USAGE_PAGE (Button) */
        0x19,
        0x01, /*     USAGE_MINIMUM (Button 1) */
        0x29,
        0x08, /*     USAGE_MAXIMUM (Button 8) */
        0x15,
        0x00, /*     LOGICAL_MINIMUM (0) */
        0x25,
        0x01, /*     LOGICAL_MAXIMUM (1) */
        0x95,
        0x08, /*     REPORT_COUNT (8) */
        0x75,
        0x01, /*     REPORT_SIZE (1) */
        0x81,
        0x02, /*     INPUT (Data,Var,Abs) */

        /* X, Y */
        0x05,
        0x01, /*     USAGE_PAGE (Generic Desktop) */
        0x09,
        0x30, /*     USAGE (X) */
        0x09,
        0x31, /*     USAGE (Y) */
        0x16,
        0x00,
        0x80, /* 	Logical Minimum (-32768) */
        0x26,
        0xFF,
        0x7F, /* 	Logical Maximum (32767) */
        0x75,
        0x10, /* 	Report Size (16), */
        0x95,
        0x02, /* 	Report Count (2), */
        0x81,
        0x02, /* 	Input (Data, Variable, Absolute) */

        /* Wheel */
        0x09,
        0x38, /*     USAGE (Wheel) */
        0x15,
        0x81, /*     LOGICAL_MINIMUM (-127) */
        0x25,
        0x7f, /*     LOGICAL_MAXIMUM (127) */
        0x75,
        0x08, /*     REPORT_SIZE (8) */
        0x95,
        0x01, /*     REPORT_COUNT (1) */
        0x81,
        0x06, /*     INPUT (Data,Var,Rel) */

        /* End */
        0xc0 /* END_COLLECTION */
};

TEST_CASE("USB::HidParser", "[core]") {
    hid::DeviceDescriptor* device_desc;
    hid::ParseResult res =
            hid::ParseReportDescriptor(test_mouse_desc, sizeof(test_mouse_desc), &device_desc);
    REQUIRE(res == hid::kParseOk);
    REQUIRE(device_desc->rep_count == 1);
    for (size_t i = 0; i < device_desc->rep_count; ++i) {
        const auto& report_desc = device_desc->report[i];
        REQUIRE(report_desc.input_count > 0);
        for (size_t j = 0; j < report_desc.input_count; j++) {
            const hid::ReportField& field = report_desc.input_fields[j];
            if (field.attr.usage ==
                hid::USAGE(hid::usage::Page::kGenericDesktop, hid::usage::GenericDesktop::kX)) {
                REQUIRE(field.attr.offset == 0x8);
                REQUIRE(field.attr.bit_sz == 16);
                REQUIRE(field.attr.logc_mm.min == -32768);
                REQUIRE(field.attr.logc_mm.max == 32767);
            } else if (field.attr.usage == hid::USAGE(hid::usage::Page::kGenericDesktop,
                                                      hid::usage::GenericDesktop::kY)) {
                REQUIRE(field.attr.offset == 0x18);
                REQUIRE(field.attr.bit_sz == 16);
                REQUIRE(field.attr.logc_mm.min == -32768);
                REQUIRE(field.attr.logc_mm.max == 32767);
            }
        }
    }
    FreeDeviceDescriptor(device_desc);
}
