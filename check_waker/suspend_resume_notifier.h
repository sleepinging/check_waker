#pragma once

#include <windows.h>
#include <powrprof.h>

#include <functional>

class SuspendResumeNotifier {
public:
    enum class Status {
        kUnkown = 0,
        kSuspending,
        kResuming,
    };

    using status_changed_handler_t = std::function<void(Status status)>;

public:
    HRESULT initialize() noexcept;

    void uninitialize() noexcept;

    void setHandler(status_changed_handler_t handler) noexcept {
        status_changed_handler_.swap(handler);
    }

private:
    void callback(ULONG type) noexcept;

    static ULONG CALLBACK winCallback(_In_opt_ PVOID Context, _In_ ULONG Type, _In_ PVOID Setting);

private:
    HPOWERNOTIFY handle_{ nullptr };
    status_changed_handler_t status_changed_handler_;
};