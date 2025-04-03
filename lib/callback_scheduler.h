#pragma once

#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
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

    template<typename T>
    std::future<T> AddTaskWithFuture(std::function<T(void)> callback, TDuration duration) {
        auto [wrapper, answer] = MakeWrapper(callback);
        AddTask(wrapper, duration);
        return answer;
    }

    template<typename T>
    std::future<T> AddTaskWithFuture(std::function<T(void)> callback, TTimePoint time_point) {
        auto [wrapper, answer] = MakeWrapper(callback);
        AddTask(wrapper, time_point);
        return answer;
    }

    void AddTask(TCallback callback, TDuration duration);
    void AddTask(TCallback callback, TTimePoint time_point);
private:
    void InitSchedulerThread();

    template<typename T>
    static std::pair<TCallback, std::future<T>> MakeWrapper(std::function<T(void)> callback) {
        auto promise = std::make_shared<std::promise<T>>();
        std::future<T> answer = promise->get_future();
        auto wrapper = [callback, promise]() {
            try {
                T result = callback();
                promise->set_value(std::move(result));
            } catch (...) {
                promise->set_exception(std::current_exception());
            }
        };
        return {std::move(wrapper), std::move(answer)};
    }
private:
    std::multimap<TTimePoint, TCallback> CallbackQueue_;

    std::queue<std::pair<TTimePoint, TCallback>> InputQueue_;

    std::condition_variable Trigger_;
    std::mutex TriggerMutex_;

    std::shared_ptr<ICallbackExecutor> Executor_;

    std::unique_ptr<std::jthread> SchedulerThread_;
};
