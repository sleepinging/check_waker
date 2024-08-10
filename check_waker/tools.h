#pragma once

#include <chrono>
#include <filesystem>

std::chrono::system_clock::time_point fixZone(std::chrono::system_clock::time_point tp) noexcept;
std::filesystem::path getCurrentPath() noexcept;