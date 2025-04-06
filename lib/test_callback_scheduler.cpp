#include "test_callback_scheduler.h"


void TSyncExecutor::Invoke(std::function<void(void)> callback) {
    callback();
}

TTestCallbackScheduler::TTestCallbackScheduler()
    : TCallbackScheduler(std::make_shared<TSyncExecutor>())
    , Now_(std::chrono::steady_clock::now())
{}

void TTestCallbackScheduler::SetNow(TTimePoint now) {
    Now_ = now;
    ForceCycle();
}

void TTestCallbackScheduler::IncDuration(TDuration period) {
    SetNow(GetNow() + period);
}

TTestCallbackScheduler::TTimePoint TTestCallbackScheduler::GetNow() const {
    return Now_;
}
