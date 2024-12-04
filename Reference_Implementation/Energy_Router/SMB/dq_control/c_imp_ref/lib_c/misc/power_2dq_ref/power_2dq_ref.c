#include <math.h>
#include "power_2dq_ref.h"

/**
 * Converts apparent power and power factor to d-q current references
 * For inductive loads where current lags voltage
 * 
 * @param apparent_power: Apparent power in VA
 * @param power_factor: Power factor (-1 to 1), negative indicates power generation
 * @param vd: d-axis voltage component
 * @param vq: q-axis voltage component
 * @param id_ref: Pointer to store calculated d-axis current reference
 * @param iq_ref: Pointer to store calculated q-axis current reference
 */
void power_to_dq_current_ref(float apparent_power, float power_factor,
                            float vd, float vq,
                            float* id_ref, float* iq_ref, 
                            bool reactive_power_is_leading)
{
    // Calculate power factor angle (in radians)
    if (power_factor < -1.0f) power_factor = -1.0f;
    if (power_factor > 1.0f)  power_factor = 1.0f;
    apparent_power = fabsf(apparent_power); // ensure positive

    float pf_angle = acosf(fabsf(power_factor));  // Add fabsf() for safety
    
    // Calculate active and reactive power
    // Sign of power_factor determines if power is being consumed or generated
    float active_power = apparent_power * power_factor;

    // Always positive for inductive load
    float reactive_power = apparent_power * sinf(pf_angle); 

    // If reactive power is leading, negate it
    if (reactive_power_is_leading) 
        reactive_power = -reactive_power;
    
    // Calculate magnitude of voltage
    float v_mag = sqrtf(vd * vd + vq * vq);
    
    // Avoid division by zero
    if (v_mag < 1.0f) {
        *id_ref = 0.0f;
        *iq_ref = 0.0f;
        return;
    }
    
    float v_squared = v_mag * v_mag;
    
    // Generic equations for single-phase, any voltage orientation
    *id_ref = 2.0 * (active_power * vd + reactive_power * vq) / v_squared;
    *iq_ref = 2.0 * (active_power * vq - reactive_power * vd) / v_squared;
}
