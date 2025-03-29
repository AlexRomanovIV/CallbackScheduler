#pragma once

#include <functional>

class ICallbackExecutor {
public:
    virtual ~ICallbackExecutor() = default;

    virtual void Invoke(std::function<void(void)>) = 0;
};
