#pragma once

#include <string>
#include <vector>
#include "base/types.h"

namespace base {

std::vector<u8> ReadFile(const std::string& filename);

}  // namespace base