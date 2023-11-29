#pragma once

#include <optional>
#include <span>
#include <string>
#include <vector>
#include "base/macros.h"
#include "base/static_vector.h"
#include "core/usb/hid_device_descriptor.h"
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

    u32 GetReportLength() const { return report_length; }

    u8 GetReportID() const { return report_id; }

private:
    bool is_absolute{};
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
    InputMouse();

    InputMouse(u16 vid, u16 pid);

    const MouseProtocol& GetProtocol() const { return protocol; }

protected:
    bool MatchInterfaceDescriptor(const libusb_interface_descriptor* interface_desc) override;

    void InitProtocol() override;

private:
    MouseProtocol protocol;
};

class OutputMouse : public OutputHIDDevice {
public:
    explicit OutputMouse(const MouseProtocol& protocol);

    ~OutputMouse() override;

private:
    MouseProtocol protocol;
};

}  // namespace aibox::usb