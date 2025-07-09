#include "StopWatch.h"

void StopWatch::Push() {
    if (!isRunning_) {
        Start();
    }
    else {
        Stop();
    }
}

void StopWatch::Start() {
    startTime_ = std::chrono::steady_clock::now();
    isRunning_ = true;
}

void StopWatch::Stop() {
    endTime_ = std::chrono::steady_clock::now();
    isRunning_ = false;
    finish_ = true;
}

double StopWatch::ElapsedSeconds() const {
    auto end = isRunning_ ? std::chrono::steady_clock::now() : endTime_;
    double s = std::chrono::duration<double>(end - startTime_).count();
    return s;
}
