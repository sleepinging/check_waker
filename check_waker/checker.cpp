#include "checker.h"

#include <Windows.h>

#include "log.h"
#include "tools.h"

namespace {
    std::string tickTimeToString(DWORD count) noexcept {
        // 获取系统 tick count
        DWORD current_count = GetTickCount();
        auto diff = current_count - count;
        // 转为系统时间
        auto now = std::chrono::system_clock::now();
        auto that_time = now - std::chrono::milliseconds(diff);
        // 2024-08-09 13:24:13
        auto time_str = std::format("{:%Y-%m-%d %H:%M:%OS}", fixZone(that_time));
        return time_str;
    }

    bool getLastInputTime(std::chrono::steady_clock::time_point& time) noexcept {
        // 用来检测键鼠操作
        LASTINPUTINFO lastInputInfo{
            .cbSize = sizeof(LASTINPUTINFO)
        };
        if (!GetLastInputInfo(&lastInputInfo)) {
            LOGF_ERROR("GetLastInputInfo failed {}", GetLastError());
            return false;
        }
        //DBGF_INFO("Last input time: {}", tickTimeToString(lastInputInfo.dwTime));
        // 获取系统 tick count
        DWORD current_count = GetTickCount();
        auto diff = current_count - lastInputInfo.dwTime;
        // 转为系统时间
        auto now = std::chrono::steady_clock::now();
        time = now - std::chrono::milliseconds(diff);
        return true;
    }

    std::chrono::system_clock::time_point steadyToSystem(std::chrono::steady_clock::time_point steady_time) noexcept {
        auto system_now = std::chrono::system_clock::now();
        auto steady_now = std::chrono::steady_clock::now();
        std::chrono::steady_clock::duration diff_ns = steady_now - steady_time;
        auto ns = system_now.time_since_epoch() - diff_ns;
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::nanoseconds(ns));
        return std::chrono::system_clock::time_point(seconds);
    }
}


bool Checker::checkOnce(bool& input_after_resume, bool& timeout) noexcept
{
    std::chrono::steady_clock::time_point last_input_time;
    if (!getLastInputTime(last_input_time)) {
        return false;
    }
    std::chrono::system_clock::time_point system_last_input_time = steadyToSystem(last_input_time);
    DBGF_INFO("Last input time: {:%Y-%m-%d %H:%M:%OS}", fixZone(system_last_input_time));

    if (last_input_time >= resume_time_) {
        input_after_resume = true;
        return true;
    }

    auto now = std::chrono::steady_clock::now();
    if (now - resume_time_ > timeout_) {
        timeout = true;
        return true;
    }

    return true;
}
