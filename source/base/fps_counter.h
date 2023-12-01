#pragma once

#include <chrono>

namespace base {

class FPSCounter {
private:
    int frame_count;
    std::chrono::duration<double> elapsed_time;
    std::chrono::time_point<std::chrono::steady_clock> last_time;

public:
    FPSCounter() : frame_count(0), elapsed_time(0) { last_time = std::chrono::steady_clock::now(); }

    void Reset() {
        last_time = std::chrono::steady_clock::now();
        frame_count = 0;
    }

    void Tick() {
        auto now = std::chrono::steady_clock::now();
        frame_count++;
        elapsed_time += now - last_time;
        last_time = now;
    }

    double GetFPS() { return frame_count / elapsed_time.count(); }
};

}  // namespace base