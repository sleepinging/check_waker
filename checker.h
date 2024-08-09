#pragma once

#include <functional>
#include <chrono>

class Checker {
public:
    Checker(std::chrono::steady_clock::duration timeout) noexcept
        : timeout_(timeout) {}

    void reset(std::chrono::steady_clock::time_point resume_time) noexcept {
        resume_time_ = resume_time;
    }

    bool checkOnce(bool& input_after_resume, bool& timeout) noexcept;

private:
    std::chrono::steady_clock::time_point resume_time_;
    std::chrono::steady_clock::duration timeout_;
};