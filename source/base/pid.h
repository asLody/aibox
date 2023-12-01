#pragma once

#include "base/types.h"

namespace base {

class PID {
public:
    PID(float kp, float ki, float kd, float min, float max, float dt)
            : kp(kp), ki(ki), kd(kd), min(min), max(max), dt(dt) {}

    float Update(float error);

    void Reset() {
        integral = 0;
        pre_error = 0;
    }

    void SetRange(float rang) {
        min = -rang;
        max = rang;
    }

private:
    const float dt;
    const float kp;
    const float ki;
    const float kd;
    float max;
    float min;
    float integral{};
    float pre_error{};
};

}  // namespace base