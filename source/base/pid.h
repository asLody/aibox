#pragma once

#include "base/types.h"

namespace base {

class PID {
public:
    PID(float kp, float ki, float kd, float max, float min, float dt)
            : kp(kp), ki(ki), kd(kd), max(max), min(min), dt(dt) {}

    float Update(float error);

    void Reset() {
        integral = 0;
        pre_error = 0;
    }

private:
    const float dt;
    const float max;
    const float min;
    const float kp;
    const float ki;
    const float kd;
    float integral{};
    float pre_error{};
};

}  // namespace base