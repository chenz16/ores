#ifndef CURRENT_REF_CONVERSION_H
#define CURRENT_REF_CONVERSION_H

#include <stdint.h>
#include <math.h>
#include <stdlib.h>

/**
 * @brief Converts power and power factor to DQ frame current references
 * 
 * @param power_ref_w Power reference in Watts (positive for charging, negative for discharging)
 * @param power_factor Power factor (0.0 to 1.0)
 * @param grid_voltage_d D-axis grid voltage component
 * @param grid_voltage_q Q-axis grid voltage component
 * @param current_ref_d Pointer to store calculated D-axis current reference
 * @param current_ref_q Pointer to store calculated Q-axis current reference
 * @return int32_t Returns 0 if successful, error code otherwise
 */
int32_t convert_power_to_dq_current_ref(
    float power_ref_w,
    float power_factor,
    float grid_voltage_d,
    float grid_voltage_q,
    float* current_ref_d,
    float* current_ref_q
);

#endif /* CURRENT_REF_CONVERSION_H */
