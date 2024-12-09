#ifndef POWER_2DQ_REF_H
#define POWER_2DQ_REF_H

#include <stdbool.h>

/**
 * @brief Converts apparent power and power factor to d-q current references
 * 
 * @param apparent_power Apparent power in VA
 * @param power_factor Power factor (-1 to 1), negative indicates power generation
 * @param vd d-axis voltage component
 * @param vq q-axis voltage component
 * @param id_ref Pointer to store calculated d-axis current reference
 * @param iq_ref Pointer to store calculated q-axis current reference
 * 
 * @note This function assumes an inductive load where current lags voltage
 */
void power_to_dq_current_ref(float apparent_power, float power_factor,
                            float vd, float vq,
                            float* id_ref, float* iq_ref, 
                            bool reactive_power_is_leading);

#endif /* POWER_2DQ_REF_H */
