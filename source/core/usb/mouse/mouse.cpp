#include "core/usb/hid-parser/report.h"
#include "core/usb/hid-parser/usages.h"
#include "core/usb/mouse/mouse.h"

namespace aibox::usb {

void MouseProtocol::ParseReportDescriptor(const hid::DeviceDescriptor* descriptor) {
    for (int i = 0; i < descriptor->rep_count; ++i) {
        const auto& report = descriptor->report[i];
        for (size_t j = 0; j < report.input_count; j++) {
            const hid::ReportField& field = report.input_fields[j];
            if (field.attr.usage ==
                hid::USAGE(hid::usage::Page::kGenericDesktop, hid::usage::GenericDesktop::kX)) {
                if (field.flags & hid::FieldTypeFlags::kAbsolute) {
                    position_x = field.attr;
                } else {
                    movement_x = field.attr;
                }
            } else if (field.attr.usage == hid::USAGE(hid::usage::Page::kGenericDesktop,
                                                      hid::usage::GenericDesktop::kY)) {
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
        if (position_x || movement_x) {
            is_absolute = position_x.has_value();
            hid_report_length = report.input_byte_sz;
            hid_report_id = report.report_id;
            break;
        }
    }
    if (!position_x && !movement_x) {
        throw std::runtime_error("MouseProtocol: no input found");
    }
}

void MouseProtocol::Decode(MouseReport& report, std::span<u8> data) const {
    const auto extract = [&](const std::optional<hid::Attributes>& attr, double* value) {
        if (!attr) return;
        hid::ExtractAsUnitType(data.data(), data.size(), *attr, value);
    };
    if (is_absolute) {
        extract(position_x, &report.x);
        extract(position_y, &report.y);
    } else {
        extract(movement_x, &report.x);
        extract(movement_y, &report.y);
    }
    extract(scroll_v, &report.scroll);
    report.buttons.resize(buttons.size());
    for (size_t i = 0; i < buttons.size(); i++) {
        extract(buttons[i], &report.buttons[i]);
    }
}

void MouseProtocol::Encode(std::span<u8> data, const MouseReport& report) const {
    if (hid_report_id != 0) {
        data[0] = hid_report_id;
    }
    const auto insert = [&](const std::optional<hid::Attributes>& attr, double value) {
        if (!attr) return;
        hid::InsertAsUnitType(data.data(), data.size(), *attr, value);
    };
    if (is_absolute) {
        insert(position_x, report.x);
        insert(position_y, report.y);
    } else {
        insert(movement_x, report.x);
        insert(movement_y, report.y);
    }
    insert(scroll_v, report.scroll);
    for (size_t i = 0; i < report.buttons.size(); i++) {
        insert(buttons[i], report.buttons[i]);
    }
}

}  // namespace aibox::usb
