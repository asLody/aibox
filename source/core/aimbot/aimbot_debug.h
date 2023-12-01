#pragma once

#include <string>
#include <vector>
#include "base/types.h"
#include "core/model/yolo/yolo.h"

namespace aibox::aimbot {

void WriteMarkedImage(const std::string& path,
                      const u8* data,
                      u32 width,
                      u32 height,
                      const std::vector<model::Box>& boxes,
                      s32 target_index);

}
