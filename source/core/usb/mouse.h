#pragma once

#include <optional>
#include <span>
#include <string>
#include <vector>
#include "base/macros.h"
#include "core/usb/hid_device_descriptor.h"
#include "core/usb/input_hid_device.h"
#include "core/usb/output_hid_device.h"

namespace aibox::usb {

struct MouseReport {
    bool is_absolute;
    s32 x, y;
    s32 scroll;
};

class MouseProtocol {
public:
    void ParseReportDescriptor(const hid::ReportDescriptor& descriptor);

    void Decode(MouseReport& report, std::span<u8> data);

private:
    u32 report_length{};
    u8 report_id{};
    std::optional<hid::Attributes> movement_x;
    std::optional<hid::Attributes> movement_y;
    std::optional<hid::Attributes> position_x;
    std::optional<hid::Attributes> position_y;
    std::optional<hid::Attributes> scroll_v;
    std::vector<hid::Attributes> buttons;
};

class InputMouse : public InputHIDDevice {
public:
    InputMouse(u16 vid, u16 pid);

protected:
    bool MatchInterfaceDescriptor(const libusb_interface_descriptor* interface_desc) override;
};

class OutputMouse : public OutputHIDDevice {
public:
    explicit OutputMouse();

    ~OutputMouse() override;
};

}  // namespace aibox::usb