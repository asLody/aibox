#include <algorithm>
#include "base/pid.h"

namespace base {

float PID::Update(float error) {
    float pout = kp * error;
    integral += error * dt;
    float iout = ki * integral;
    float derivative = (error - pre_error) / dt;
    float dout = kd * derivative;

    float output = pout + iout + dout;
    output = std::clamp(output, min, max);

    pre_error = error;
    return output;
}

}  // namespace base
