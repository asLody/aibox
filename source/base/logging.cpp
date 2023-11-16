#include "base/logging.h"

namespace base::log {

void FmtLogMessageImpl(Level log_level,
                       const char* filename,
                       unsigned int line_num,
                       const char* function,
                       const char* format,
                       const fmt::format_args& args) {
    const auto message_body = fmt::vformat(format, args);
    const auto message = fmt::format("{}:{}:{}: {}", filename, line_num, function, message_body);
    fmt::print(log_level >= Level::Warning ? stderr : stdout, "{}\n", message);
}

void Panic(const std::string& reason) {
    fmt::print("Panic: {}\n", reason);
    abort();
}

}  // namespace base::log