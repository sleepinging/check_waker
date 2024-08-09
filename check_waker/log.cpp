#include "log.h"

#include <chrono>

void simple_log(LogLevel level, std::string_view line) noexcept {
    auto now = std::chrono::system_clock::now();
    auto time_str = std::format("{:%Y-%m-%d %H:%M:%S}", now);

    const static char* level_str[] = {
        "DEBUG",
        "INFO",
        "WARN",
        "ERROR"
    };

    auto line_str = std::format("[{}]\t({})\t {}", level_str[static_cast<int>(level)], time_str, line);

    switch (level)
    {
    case LogLevel::kDebug:
    case LogLevel::kInfo:
    case LogLevel::kWarn:
        std::cout << line_str << std::endl;
        break;
    case LogLevel::kError:
        std::cerr << line_str << std::endl;
        break;
    default:
        std::cout << line_str << std::endl;
        break;
    }
}