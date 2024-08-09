#include "tools.h"

std::chrono::system_clock::time_point fixZone(std::chrono::system_clock::time_point tp) noexcept
{
    // ÐÞÕýÊ±Çø
    long tz{};
    _get_timezone(&tz);
    auto zoned_time = tp - std::chrono::seconds(tz);
    return zoned_time;
}
