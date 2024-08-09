#pragma once

#include <chrono>

std::chrono::system_clock::time_point fixZone(std::chrono::system_clock::time_point tp) noexcept;