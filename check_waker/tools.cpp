#include "tools.h"

#include <Windows.h>

std::chrono::system_clock::time_point fixZone(std::chrono::system_clock::time_point tp) noexcept
{
    // 修正时区
    long tz{};
    _get_timezone(&tz);
    auto zoned_time = tp - std::chrono::seconds(tz);
    return zoned_time;
}

std::filesystem::path getCurrentPath() noexcept
{
    wchar_t path_str[MAX_PATH]{ 0 };
    if (GetModuleFileNameW(NULL, path_str, MAX_PATH) == 0) {
        return {};
    }
    std::filesystem::path path(path_str);
    return path.parent_path();
}
