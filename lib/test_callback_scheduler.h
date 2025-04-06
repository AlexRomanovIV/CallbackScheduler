#pragma once

#include "callback_scheduler.h"
#include "callback_executor.h"

class TSyncExecutor : public ICallbackExecutor {
public:
    void Invoke(std::function<void(void)> callback) override;
};

class TTestCallbackScheduler : public TCallbackScheduler {
public:
    TTestCallbackScheduler();
    void SetNow(TTimePoint now);
    void IncDuration(TDuration period);
protected:
    TTimePoint GetNow() const override;
private:
    std::atomic<TTimePoint> Now_;
};
