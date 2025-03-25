#include "callback_scheduler.h"

void TCallbackScheduler::AddTask(std::function<void(void)>, std::chrono::steady_clock::duration) {

}

void TCallbackScheduler::AddTask(std::function<void(void)>, std::chrono::time_point<std::chrono::steady_clock>) {

}
