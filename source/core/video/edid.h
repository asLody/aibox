#pragma once

#include "base/types.h"

namespace aibox::video {

constexpr size_t EDID_BLOCK_SIZE = 128;

constexpr u8 EDID_FORCE_GBR24[] = {
        0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x49, 0x70, 0x88, 0x35, 0x01, 0x00, 0x00,
        0x00, 0x2d, 0x1f, 0x01, 0x03, 0x80, 0x78, 0x44, 0x78, 0x0a, 0xcf, 0x74, 0xa3, 0x57, 0x4c,
        0xb0, 0x23, 0x09, 0x48, 0x4c, 0x21, 0x08, 0x00, 0x61, 0x40, 0x01, 0x01, 0x81, 0x00, 0x95,
        0x00, 0xa9, 0xc0, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3a, 0x80, 0x18, 0x71, 0x38,
        0x2d, 0x40, 0x58, 0x2c, 0x45, 0x00, 0x20, 0xc2, 0x31, 0x00, 0x00, 0x1e, 0x01, 0x1d, 0x00,
        0x72, 0x51, 0xd0, 0x1e, 0x20, 0x6e, 0x28, 0x55, 0x00, 0x20, 0xc2, 0x31, 0x00, 0x00, 0x1e,
        0x00, 0x00, 0x00, 0xfc, 0x00, 0x52, 0x4b, 0x2d, 0x55, 0x48, 0x44, 0x0a, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x3b, 0x46, 0x1f, 0x8c, 0x3c, 0x00, 0x0a,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0xa7, 0x02, 0x03, 0x2a, 0xc1, 0x51, 0x07, 0x16,
        0x14, 0x05, 0x01, 0x03, 0x12, 0x13, 0x84, 0x22, 0x1f, 0x90, 0x5d, 0x5e, 0x5f, 0x60, 0x61,
        0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00, 0x67, 0x03, 0x0c, 0x00, 0x30, 0x00, 0x00,
        0x44, 0xe3, 0x05, 0x03, 0x01, 0x02, 0x3a, 0x80, 0x18, 0x71, 0x38, 0x2d, 0x40, 0x58, 0x2c,
        0x45, 0x00, 0x20, 0xc2, 0x31, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xb8};

constexpr u8 EDID_FORCE_GBR24_60HZ[] = {
        0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x49, 0x70, 0x88, 0x35, 0x01, 0x00, 0x00,
        0x00, 0x2d, 0x1f, 0x01, 0x03, 0x80, 0x78, 0x44, 0x78, 0x0a, 0xcf, 0x74, 0xa3, 0x57, 0x4c,
        0xb0, 0x23, 0x09, 0x48, 0x4c, 0x21, 0x08, 0x00, 0x61, 0x40, 0x01, 0x01, 0x81, 0x00, 0x95,
        0x00, 0xa9, 0xc0, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3a, 0x80, 0x18, 0x71, 0x38,
        0x2d, 0x40, 0x58, 0x2c, 0x45, 0x00, 0x20, 0xc2, 0x31, 0x00, 0x00, 0x1e, 0x01, 0x1d, 0x00,
        0x72, 0x51, 0xd0, 0x1e, 0x20, 0x6e, 0x28, 0x55, 0x00, 0x20, 0xc2, 0x31, 0x00, 0x00, 0x1e,
        0x00, 0x00, 0x00, 0xfc, 0x00, 0x52, 0x4b, 0x2d, 0x55, 0x48, 0x44, 0x0a, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x3b, 0x46, 0x1f, 0x8c, 0x3c, 0x00, 0x0a,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0xa7, 0x02, 0x03, 0x20, 0xc1, 0x47, 0x07, 0x05,
        0x01, 0x03, 0x84, 0x90, 0x61, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00, 0x67, 0x03,
        0x0c, 0x00, 0x30, 0x00, 0x00, 0x44, 0xe3, 0x05, 0x03, 0x01, 0x02, 0x3a, 0x80, 0x18, 0x71,
        0x38, 0x2d, 0x40, 0x58, 0x2c, 0x45, 0x00, 0x20, 0xc2, 0x31, 0x00, 0x00, 0x1e, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xd6};

}  // namespace aibox::video