#include "dq_to_modulation.h"
#include <math.h>

modulation_result_t dq_to_modulation_calculate(dq_voltage_t dq_voltage) {
    modulation_result_t result = {0};
    
    // Calculate magnitude of the voltage vector
    float v_magnitude = sqrtf(dq_voltage.vd * dq_voltage.vd + dq_voltage.vq * dq_voltage.vq);

    if (dq_voltage.vdc < 1.0f) {
        dq_voltage.vdc = 1.0f;
    }
    result.index =  v_magnitude / dq_voltage.vdc;

    // Calculate phase shift and wrap to [-π, π]
    result.phase_shift = atan2f(dq_voltage.vq, dq_voltage.vd);
    
    // Wrap phase to [-π, π]
    while (result.phase_shift > M_PI) {
        result.phase_shift -= 2.0f * M_PI;
    }
    while (result.phase_shift < -M_PI) {
        result.phase_shift += 2.0f * M_PI;
    }
    
    // Check if modulation index is within valid range (0 to 1)
    result.valid = (result.index >= 0.0f && result.index <= 1.2f);
    
    return result;
}