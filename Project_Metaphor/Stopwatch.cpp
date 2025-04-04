#include "Stopwatch.h"
#include <iostream>

Stopwatch::Stopwatch() : running(false) {
}

void Stopwatch::start() {
    start_time = clock::now();
    running = true;
    std::cout << "Stopwatch started.\n";
}

void Stopwatch::stop() {
    if (running) {
        end_time = clock::now();
        running = false;
        std::cout << "Stopwatch stopped.\n";
    }
}

double Stopwatch::elapsedSeconds() {
    if (running) {
        time_point now = clock::now();
        std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time);
        return duration.count() / 1000.0;
    }
    else {
        std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        return duration.count() / 1000.0;
    }
}

void Stopwatch::reset() {
    running = false;
    start_time = time_point();
    end_time = time_point();
    std::cout << "Stopwatch reset.\n";
}