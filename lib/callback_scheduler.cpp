#include "callback_scheduler.h"

#include <future>

TCallbackScheduler::TCallbackScheduler(std::shared_ptr<ICallbackExecutor> executor)
: Executor_(executor)
{
    InitSchedulerThread();
}

void TCallbackScheduler::InitSchedulerThread() {
    std::promise<void> initialized;
    auto readyToStart = initialized.get_future();
    auto schedulerFunc = [this](std::stop_token stop_token, std::promise<void> initialized) {
        std::unique_lock lock(TriggerMutex_);
        initialized.set_value();
        while (!stop_token.stop_requested()) {
            if (CallbackQueue_.empty()) {
                Trigger_.wait(lock);
            } else {
                Trigger_.wait_until(lock, CallbackQueue_.begin()->first);
            }

            while (!InputQueue_.empty()) {
                CallbackQueue_.insert(InputQueue_.front());
                InputQueue_.pop();
            }

            auto now = std::chrono::steady_clock::now();
            while (!CallbackQueue_.empty() && now >= CallbackQueue_.begin()->first) {
                Executor_->Invoke(CallbackQueue_.begin()->second);
                CallbackQueue_.erase(CallbackQueue_.begin());
            }
        }
    };
    SchedulerThread_ = std::make_unique<std::jthread>(schedulerFunc, std::move(initialized));
    readyToStart.wait();
}

TCallbackScheduler::~TCallbackScheduler() {
    SchedulerThread_->request_stop();
    std::lock_guard guard(TriggerMutex_);
    Trigger_.notify_one();
}

void TCallbackScheduler::AddTask(std::function<void(void)> callback, std::chrono::steady_clock::duration duration) {
    AddTask(callback, std::chrono::steady_clock::now() + duration);
}

void TCallbackScheduler::AddTask(std::function<void(void)> callback, std::chrono::time_point<std::chrono::steady_clock> timePoint) {
    std::lock_guard guard(TriggerMutex_);

    InputQueue_.emplace(timePoint, callback);
    Trigger_.notify_one();
}
