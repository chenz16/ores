#ifndef WRAP_ANGLE_H
#define WRAP_ANGLE_H

#include <math.h>

/**
 * @brief Wraps an angle to the range [-π, π]
 * @param angle The input angle in radians
 * @return The wrapped angle in radians, between -π and π
 */
static inline double wrap_angle(double angle) {
    angle = fmod(angle + M_PI, 2.0 * M_PI);
    if (angle < 0) {
        angle += 2.0 * M_PI;
    }
    return angle - M_PI;
}

#endif /* WRAP_ANGLE_H */