// check_starter.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <Windows.h>

#include <iostream>
#include <chrono>
#include <format>
#include <functional>
#include <atomic>
#include <filesystem>

#include "suspend_resume_notifier.h"
#include "log.h"
#include "checker.h"
#include "tools.h"

#ifndef _DEBUG
#pragma comment( linker, "/subsystem:windows /entry:mainCRTStartup" )
#endif // !_DEBUG


extern "C"
{
    int WINAPI MessageBoxTimeoutA(IN HWND hWnd, IN LPCSTR lpText, IN LPCSTR lpCaption, IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);
    int WINAPI MessageBoxTimeoutW(IN HWND hWnd, IN LPCWSTR lpText, IN LPCWSTR lpCaption, IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);
};
#ifdef UNICODE
#define MessageBoxTimeout MessageBoxTimeoutW
#else
#define MessageBoxTimeout MessageBoxTimeoutA
#endif

#define ID_TIMEOUT 32000

Checker checker(std::chrono::seconds(30));
std::atomic_uint32_t g_session_id = 0;

bool doScriptAndSleep() noexcept {
    // 调用用户自定义脚本
    constexpr auto kScriptName = L"before_sleep.bat";
    if (std::filesystem::exists(kScriptName)) {
        if (auto c = system("before_sleep.bat"); c != 0) {
            LOGF_WARN("script exit {} != 0, cancel", c);
            return false;
        }
    }
    // 让系统进入睡眠状态
    LOGF_INFO("Sleep now");
    return system("psshutdown64.exe -d -t 0") == 0;
}

bool confirmAndSleep() noexcept {
    // 5分钟没有点击确认就自动睡眠
    auto sleep_time = std::chrono::system_clock::now() + std::chrono::minutes(5);
    auto str = std::format(L"检测到没有操作，将会在{:%Y-%m-%d %H:%M:%OS}重新睡眠, 关闭此窗口取消睡眠", fixZone(sleep_time));
    auto r = MessageBoxTimeout(NULL, str.c_str(), L"提示", MB_OK, 0, 5 * 60 * 1000);
    if (r == ID_TIMEOUT) {
        LOGF_INFO("Timeout, try sleep");
        if (!doScriptAndSleep()) {
            LOGF_ERROR("Failed to sleep");
            return false;
        }
        return true;
    }
    return false;
}

bool checkLoop(std::chrono::steady_clock::time_point resume_time, uint32_t session_id) noexcept {
    checker.reset(resume_time);
    while (true)
    {
        bool timeout = false;
        bool user_input = false;
        if (!checker.checkOnce(user_input, timeout)) {
            LOGF_WARN("Failed to check user input");
            break;
        }

        if (user_input) {
            LOGF_INFO("User input detected, exit check loop");
            break;
        }

        if (session_id != g_session_id.load()) {
            LOGF_INFO("Session changed, exit check loop");
            break;
        }

        if (timeout) {
            LOGF_INFO("Timeout, sleep now");
            confirmAndSleep();
            break;
        }

        Sleep(1000);

    }
    return true;
}

int main()
{
    // 微软的运行时库没有自动调用 tzset，会导致时区获取错误，所以这里手动调用一下
    _tzset();
    volatile uint32_t resume_count = 0;

    SuspendResumeNotifier notifier;
    notifier.setHandler([&](SuspendResumeNotifier::Status status) {
        switch (status) {
        case SuspendResumeNotifier::Status::kSuspending:
            LOGF_INFO("Suspending");
            ++g_session_id;
            break;
        case SuspendResumeNotifier::Status::kResuming:
            LOGF_INFO("Resuming");
            ++resume_count;
            ++g_session_id;
            break;
        default:
            break;
        }
        });

    uint32_t last_resume_count = 0;
    if (auto hr = notifier.initialize(); FAILED(hr)) {
        LOGF_ERROR("Failed to initialize notifier: {:X}", hr);
        return 1;
    }

    LOGF_INFO("Check waker started");

    //等待被唤醒
    while (true) {
        //DBGF_INFO("Waiting for resume");
        if (last_resume_count == resume_count) {
            Sleep(1000);
            continue;
        }

        LOGF_INFO("Resume count: {}", (uint32_t)resume_count);
        last_resume_count = resume_count;

        auto resume_time = std::chrono::system_clock::now();
        checkLoop(std::chrono::steady_clock::now(), g_session_id.load());
        LOGF_INFO("recheck");
    }


    notifier.uninitialize();
    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
