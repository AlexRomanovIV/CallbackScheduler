#pragma once

#include "callback_scheduler.h"
#include "callback_executor.h"

class TSyncExecutor : public ICallbackExecutor {
public:
    void Invoke(std::function<void(void)> callback) final;
};

class TTestCallbackScheduler : public TCallbackScheduler {
public:
    TTestCallbackScheduler();
    void SetNow(TTimePoint now);
    void IncDuration(TDuration period);
protected:
    TTimePoint GetNow() const final;
private:
    std::atomic<TTimePoint> Now_;
};
