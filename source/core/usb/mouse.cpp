#include "core/usb/hid-parser/report.h"
#include "core/usb/hid-parser/usages.h"
#include "core/usb/mouse.h"

namespace aibox::usb {

InputMouse::InputMouse(u16 vid, u16 pid) : InputHIDDevice(vid, pid) {}

bool InputMouse::MatchInterfaceDescriptor(const libusb_interface_descriptor* interface_desc) {
    if (!InputHIDDevice::MatchInterfaceDescriptor(interface_desc)) {
        return false;
    }
    return interface_desc->bInterfaceProtocol == 2 /* mouse */;
}

OutputMouse::OutputMouse() = default;

OutputMouse::~OutputMouse() { Close(); }

void MouseProtocol::ParseReportDescriptor(const hid::ReportDescriptor& descriptor) {
    report_length = descriptor.input_byte_sz;
    report_id = descriptor.report_id;

    for (size_t j = 0; j < descriptor.input_count; j++) {
        const hid::ReportField& field = descriptor.input_fields[j];
        if (field.attr.usage ==
            hid::USAGE(hid::usage::Page::kGenericDesktop, hid::usage::GenericDesktop::kX)) {
            if (field.flags & hid::FieldTypeFlags::kAbsolute) {
                position_x = field.attr;
            } else {
                movement_x = field.attr;
            }
        } else if (field.attr.usage ==
                   hid::USAGE(hid::usage::Page::kGenericDesktop, hid::usage::GenericDesktop::kY)) {
            if (field.flags & hid::FieldTypeFlags::kAbsolute) {
                position_y = field.attr;
            } else {
                movement_y = field.attr;
            }
        } else if (field.attr.usage == hid::USAGE(hid::usage::Page::kGenericDesktop,
                                                  hid::usage::GenericDesktop::kWheel)) {
            scroll_v = field.attr;
        } else if (field.attr.usage.page == hid::usage::Page::kButton) {
            buttons.push_back(field.attr);
        }
    }
}

void MouseProtocol::Decode(MouseReport& report, std::span<u8> data) {
    report.is_absolute = movement_x.has_value();
    if (report.is_absolute) {
        if (movement_x) {
            hid::ExtractUint(data.data(), data.size(), movement_x.value(), (u32*)&report.x);
        }
        if (movement_y) {
            hid::ExtractUint(data.data(), data.size(), movement_y.value(), (u32*)&report.y);
        }
    } else {
        if (position_x) {
            hid::ExtractUint(data.data(), data.size(), position_x.value(), (u32*)&report.x);
        }
        if (position_y) {
            hid::ExtractUint(data.data(), data.size(), position_y.value(), (u32*)&report.y);
        }
    }
    if (scroll_v) {
        hid::ExtractUint(data.data(), data.size(), scroll_v.value(), (u32*)&report.scroll);
    }
}

}  // namespace aibox::usb
