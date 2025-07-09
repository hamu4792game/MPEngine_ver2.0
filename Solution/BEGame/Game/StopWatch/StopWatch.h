#pragma once
#include <chrono>

class StopWatch {
public:
    StopWatch() = default;
    ~StopWatch() = default;

    void Push();

    double ElapsedSeconds() const;
    bool GetFinish() const { return finish_; }

private:
    void Start();
    void Stop();
private:
    std::chrono::steady_clock::time_point startTime_{};
    std::chrono::steady_clock::time_point endTime_{};
    bool isRunning_ = false;
    bool finish_ = false;
};