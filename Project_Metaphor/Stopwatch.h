#pragma once

#include <chrono>

class Stopwatch {
private:
    typedef std::chrono::steady_clock clock;
    typedef std::chrono::time_point<clock> time_point;

    time_point start_time;
    time_point end_time;
    bool running;

public:
    Stopwatch();

    void start();
    void stop();
    double elapsedSeconds();
    void reset();
};
