#pragma once

#include <vector>
#include <string>

namespace aibox::hid {

struct __attribute__ ((packed)) MouseReport {
    uint8_t buttons;
    int16_t x;
    int16_t y;
    int8_t wheel;
};

struct MouseConfig {
    uint16_t vendor_id;
    uint16_t product_id;
    std::string manufacturer;
    std::string product;
    std::string serial;
    std::string configuration;
    uint32_t max_power;
    uint16_t device_version;
};

class Mouse {
public:
    Mouse() = default;

    static MouseConfig GetDefaultConfig();

    void SetConfig(const MouseConfig &config_) {
        config = config_;
    }

    MouseConfig GetConfig() const {
        return config;
    }

    void Attach(int fd);

    std::vector<char> GetReportDescriptor() const;

    void MoveTo(int16_t x, int16_t y);

    void MoveWheel(int8_t y);

    bool SendReport();

private:
    int hid_fd = -1;
    MouseConfig config{GetDefaultConfig()};
    uint8_t pressed_buttons{};
    int16_t cur_x{}, cur_y{};
    int8_t wheel_y{};
};

}