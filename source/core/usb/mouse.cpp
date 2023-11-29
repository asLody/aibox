#include "core/usb/hid-parser/report.h"
#include "core/usb/hid-parser/usages.h"
#include "core/usb/mouse.h"

namespace aibox::usb {

InputMouse::InputMouse() : InputHIDDevice() {}

InputMouse::InputMouse(u16 vid, u16 pid) : InputHIDDevice(vid, pid) {}

bool InputMouse::MatchInterfaceDescriptor(const libusb_interface_descriptor* interface_desc) {
    if (!InputHIDDevice::MatchInterfaceDescriptor(interface_desc)) {
        return false;
    }
    return interface_desc->bInterfaceProtocol == 2 /* mouse */;
}

void InputMouse::InitProtocol() {
    const auto& desc = GetDescriptor().report_descriptor;
    protocol.ParseReportDescriptor(desc->report[0]);
}

OutputMouse::OutputMouse(const MouseProtocol& protocol) : protocol(protocol) {}

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
            if (buttons.size() < kMaxMouseNumButtons) {
                buttons.push_back(field.attr);
            }
        }
    }
    is_absolute = movement_x.has_value();
}

void MouseProtocol::Decode(MouseReport& report, std::span<u8> data) const {
    const auto extract = [&](const std::optional<hid::Attributes>& attr, double* value) {
        if (!attr) return;
        hid::ExtractAsUnitType(data.data(), data.size(), *attr, value);
    };
    if (is_absolute) {
        extract(movement_x, &report.x);
        extract(movement_y, &report.y);
    } else {
        extract(position_x, &report.x);
        extract(position_y, &report.y);
    }
    extract(scroll_v, &report.scroll);
    report.buttons.resize(buttons.size());
    for (size_t i = 0; i < buttons.size(); i++) {
        extract(buttons[i], &report.buttons[i]);
    }
}

void MouseProtocol::Encode(std::span<u8> data, const MouseReport& report) const {
    const auto insert = [&](const std::optional<hid::Attributes>& attr, double value) {
        if (!attr) return;
        hid::InsertAsUnitType(data.data(), data.size(), *attr, value);
    };
    if (is_absolute) {
        insert(movement_x, report.x);
        insert(movement_y, report.y);
    } else {
        insert(position_x, report.x);
        insert(position_y, report.y);
    }
    insert(scroll_v, report.scroll);
    for (size_t i = 0; i < report.buttons.size(); i++) {
        insert(buttons[i], report.buttons[i]);
    }
}

}  // namespace aibox::usb
