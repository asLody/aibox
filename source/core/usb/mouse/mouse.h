#pragma once

#include <optional>
#include <span>
#include <string>
#include <vector>
#include "base/macros.h"
#include "base/static_vector.h"
#include "core/usb/hid_gadget_descriptor.h"
#include "core/usb/input_hid_device.h"
#include "core/usb/output_hid_device.h"

namespace aibox::usb {

constexpr size_t kMaxMouseNumButtons = 16;

struct MouseReport {
    double x, y;
    double scroll;
    base::static_vector<double, kMaxMouseNumButtons> buttons{};
};

class MouseProtocol {
public:
    MouseProtocol() = default;

    void ParseReportDescriptor(const hid::ReportDescriptor& descriptor);

    void Decode(MouseReport& report, std::span<u8> data) const;

    void Encode(std::span<u8> data, const MouseReport& report) const;

    [[nodiscard]] u32 GetReportLength() const { return report_length; }

    [[nodiscard]] u8 GetReportID() const { return report_id; }

    [[nodiscard]] u32 GetButtonUsage(u32 index) const { return buttons[index].usage.usage; }

private:
    bool is_absolute{};
    u8 report_id{};
    u32 report_length{};
    std::optional<hid::Attributes> movement_x;
    std::optional<hid::Attributes> movement_y;
    std::optional<hid::Attributes> position_x;
    std::optional<hid::Attributes> position_y;
    std::optional<hid::Attributes> scroll_v;
    std::vector<hid::Attributes> buttons;
};

}  // namespace aibox::usb