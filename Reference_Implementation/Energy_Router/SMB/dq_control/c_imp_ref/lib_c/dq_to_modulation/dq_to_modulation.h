#ifndef DQ_TO_MODULATION_H
#define DQ_TO_MODULATION_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Structure to hold dq voltage components and DC bus voltage
 */
typedef struct {
    float vd;         // d-axis voltage component
    float vq;         // q-axis voltage component
    float vdc;        // DC bus voltage
} dq_voltage_t;

/**
 * @brief Structure to hold modulation results
 */
typedef struct {
    float index;           // Modulation index
    float phase_shift;     // Phase shift in radians
    bool valid;           // Indicates if the calculation was successful
} modulation_result_t;

/**
 * @brief Calculates modulation parameters from dq voltages
 * @param dq_voltage Input voltage components and DC bus voltage
 * @return Modulation index and phase shift
 */
modulation_result_t dq_to_modulation_calculate(dq_voltage_t dq_voltage);


#endif /* DQ_TO_MODULATION_H */
