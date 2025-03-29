#pragma once

#include <thread>
#include <functional>
#include <vector>

#include "callback_executor.h"

class TSimpleThreadExecutor : public ICallbackExecutor {
public:

    void Invoke(std::function<void(void)> callback) override;

private:
    std::vector<std::jthread> Threads_;
};
