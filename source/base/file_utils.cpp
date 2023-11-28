#include <filesystem>
#include <fstream>
#include "base/file_utils.h"

namespace base {

std::vector<u8> ReadFile(const std::string& filename) {
    std::ifstream stream(filename, std::ios::in | std::ios::binary);
    if (!stream) {
        return {};
    }
    const size_t size = std::filesystem::file_size(filename);
    std::vector<u8> data(size);
    stream.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(size));
    return data;
}

bool WriteFile(const std::string& filename, u8* data, size_t size) {
    std::ofstream stream(filename, std::ios::out | std::ios::binary);
    if (!stream) {
        return false;
    }
    stream.write(reinterpret_cast<char*>(data), static_cast<std::streamsize>(size));
    return true;
}

}  // namespace base
