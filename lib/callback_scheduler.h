#pragma once

#include <functional>
#include <chrono>

class TCallbackScheduler {
public:

    void AddTask(std::function<void(void)> callback, std::chrono::steady_clock::duration duration);
    void AddTask(std::function<void(void)> callback, std::chrono::time_point<std::chrono::steady_clock> time_point);
};
