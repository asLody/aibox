#pragma once

#include <fmt/core.h>
#include "base/types.h"

namespace base::log {

enum class Level : u8 {
    Info,
    Debug,
    Warning,
    Error,
};

constexpr const char* TrimSourcePath(std::string_view path) {
    const auto pos = path.rfind("source/");
    return path.data() + (pos == std::string_view::npos ? 0 : pos + 7);
}

void FmtLogMessageImpl(Level log_level,
                       const char* filename,
                       unsigned int line_num,
                       const char* function,
                       const char* format,
                       const fmt::format_args& args);

template <typename... Args>
void FmtLogMessage(Level log_level,
                   const char* filename,
                   unsigned int line_num,
                   const char* function,
                   const char* format,
                   const Args&... args) {
    FmtLogMessageImpl(
            log_level, filename, line_num, function, format, fmt::make_format_args(args...));
}

void Panic(const std::string& reason);

}  // namespace base::log

#ifdef CONFIG_STRIP_LOG

#define LOG_INFO(...) (void(0))
#define LOG_DEBUG(...) (void(0))
#define LOG_WARNING(...) (void(0))
#define LOG_ERROR(...) (void(0))

#else

#define LOG_INFO(...)                                             \
    base::log::FmtLogMessage(base::log::Level::Info,              \
                             base::log::TrimSourcePath(__FILE__), \
                             __LINE__,                            \
                             __func__,                            \
                             __VA_ARGS__)
#define LOG_DEBUG(...)                                            \
    base::log::FmtLogMessage(base::log::Level::Debug,             \
                             base::log::TrimSourcePath(__FILE__), \
                             __LINE__,                            \
                             __func__,                            \
                             __VA_ARGS__)
#define LOG_WARNING(...)                                          \
    base::log::FmtLogMessage(base::log::Level::Warning,           \
                             base::log::TrimSourcePath(__FILE__), \
                             __LINE__,                            \
                             __func__,                            \
                             __VA_ARGS__)
#define LOG_ERROR(...)                                            \
    base::log::FmtLogMessage(base::log::Level::Error,             \
                             base::log::TrimSourcePath(__FILE__), \
                             __LINE__,                            \
                             __func__,                            \
                             __VA_ARGS__)
#endif

#define ASSERT(cond)                                                      \
    do {                                                                  \
        if (!(cond)) {                                                    \
            base::log::Panic(fmt::format("Check failed: \"{}\"", #cond)); \
        }                                                                 \
    } while (0)

#define ASSERT_MSG(cond, ...)                                               \
    do                                                                      \
        if (!(cond)) {                                                      \
            base::log::Panic(fmt::format("Assertion Failed!" __VA_ARGS__)); \
        }                                                                   \
    while (0)

#define PANIC(...) ASSERT_MSG(false, __VA_ARGS__)