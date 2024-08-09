#include "suspend_resume_notifier.h"


#pragma comment(lib, "PowrProf.lib")

HRESULT SuspendResumeNotifier::initialize() noexcept {
    DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS param{};
    param.Context = this;
    param.Callback = &SuspendResumeNotifier::winCallback;
    auto result = PowerRegisterSuspendResumeNotification(DEVICE_NOTIFY_CALLBACK, &param, &handle_);
    if (result != ERROR_SUCCESS) {
        return HRESULT_FROM_WIN32(result);
    }

    return S_OK;
}

void SuspendResumeNotifier::uninitialize() noexcept {
    if (handle_) {
        PowerUnregisterSuspendResumeNotification(handle_);
        handle_ = nullptr;
    }
}


void SuspendResumeNotifier::callback(ULONG type) noexcept {
    // PBT_APMSUSPEND、PBT_APMRESUMESUSPEND和PBT_APMRESUMEAUTOMATIC
    using Status = SuspendResumeNotifier::Status;
    Status status{ Status::kUnkown };
    switch (type) {
    case PBT_APMSUSPEND:
        status = Status::kSuspending;
        break;
    case PBT_APMRESUMESUSPEND:
        // Operation is resuming from a low-power state.
        // This message is sent after PBT_APMRESUMEAUTOMATIC if the resume is triggered by user
        // input, such as pressing a key.
        return;
    case PBT_APMRESUMEAUTOMATIC:
        status = Status::kResuming;
        break;
    default:
        break;
    }

    if (status_changed_handler_) {
        status_changed_handler_(status);
    }
}

ULONG CALLBACK SuspendResumeNotifier::winCallback(_In_opt_ PVOID Context, _In_ ULONG Type, _In_ PVOID Setting) {
    auto that = static_cast<SuspendResumeNotifier*>(Context);
    if (!that) {
        // 可能是析构之前没有调用uninit
        return ERROR_INVALID_PARAMETER;
    }
    that->callback(Type);
    return 0;
}
