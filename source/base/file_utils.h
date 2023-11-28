#pragma once

#include <string>
#include <vector>
#include "base/types.h"

namespace base {

std::vector<u8> ReadFile(const std::string& filename);

bool WriteFile(const std::string& filename, u8* data, size_t size);

}  // namespace base