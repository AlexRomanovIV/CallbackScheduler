#pragma once

#include <chrono>
#include <condition_variable>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "callback_executor.h"

class TCallbackScheduler {
public:
    using TTimePoint = std::chrono::time_point<std::chrono::steady_clock>;
    using TDuration = std::chrono::steady_clock::duration;
    using TCallback = std::function<void(void)>;
public:
    TCallbackScheduler(std::shared_ptr<ICallbackExecutor> executor);
    ~TCallbackScheduler();

    void AddTask(TCallback callback, TDuration duration);
    void AddTask(TCallback callback, TTimePoint time_point);
private:
    void InitSchedulerThread();
private:
    std::multimap<TTimePoint, TCallback> CallbackQueue_;

    std::queue<std::pair<TTimePoint, TCallback>> InputQueue_;

    std::condition_variable Trigger_;
    std::mutex TriggerMutex_;

    std::unique_ptr<std::jthread> SchedulerThread_;

    std::shared_ptr<ICallbackExecutor> Executor_;
};
