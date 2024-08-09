#pragma once

#include <iostream>
#include <format>

enum class LogLevel {
    kDebug,
    kInfo,
    kWarn,
    kError
};

void simple_log(LogLevel level, std::string_view line) noexcept;

#define DBGF_INFO(...) simple_log(LogLevel::kDebug, std::format(__VA_ARGS__));
#define LOGF_INFO(...) simple_log(LogLevel::kInfo, std::format(__VA_ARGS__));
#define LOGF_WARN(...) simple_log(LogLevel::kWarn, std::format(__VA_ARGS__));
#define LOGF_ERROR(...) simple_log(LogLevel::kError, std::format(__VA_ARGS__));