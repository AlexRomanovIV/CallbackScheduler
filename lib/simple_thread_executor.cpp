#include "simple_thread_executor.h"

void TSimpleThreadExecutor::Invoke(std::function<void(void)> callback) {
    Threads_.emplace_back(callback);
}
