#ifndef VCO_CONTROLLER_H
#define VCO_CONTROLLER_H

#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

// Error codes for VCO controller operations
typedef enum {
    VCO_ERROR_NONE = 0,
    VCO_ERROR_NULL_POINTER,
    VCO_ERROR_INVALID_PARAMETER
} vco_error_t;

// VCO controller structure
typedef struct {
    // Parameters
    float ts;              // Sampling time (seconds)
    float nominal_freq;    // Nominal frequency (Hz)
    float k0;             // VCO gain factor (Hz/V)
    
    // States
    float phase;           // Total phase (nominal + correction) (radians)
    float phase_nominal;   // Phase from nominal frequency (radians)
    float phase_correction;// Phase from frequency correction (radians)
    float frequency;       // Total frequency (nominal + correction) (Hz)
    float freq_correction; // Frequency adjustment from control signal (Hz)
} vco_controller_t;

/**
 * @brief Initialize VCO controller
 * @param vco Pointer to VCO controller structure
 * @param ts Sampling time in seconds
 * @param nominal_freq Nominal frequency in Hz
 * @param k0 VCO gain factor in Hz/V
 * @return Error code indicating success or failure
 */
vco_error_t vco_controller_init(vco_controller_t* vco, 
                               float ts, 
                               float nominal_freq,
                               float k0);

/**
 * @brief Update VCO state with new control signal
 * @param vco Pointer to VCO controller structure
 * @param control_signal Control signal from PI controller
 * @return Error code indicating success or failure
 */
vco_error_t vco_controller_update(vco_controller_t* vco, float control_signal);

/**
 * @brief Reset VCO controller to initial state
 * @param vco Pointer to VCO controller structure
 * @return Error code indicating success or failure
 */
vco_error_t vco_controller_reset(vco_controller_t* vco);

/**
 * @brief Set new nominal frequency
 * @param vco Pointer to VCO controller structure
 * @param nominal_freq New nominal frequency in Hz
 * @return Error code indicating success or failure
 */
vco_error_t vco_controller_set_nominal_freq(vco_controller_t* vco, float nominal_freq);

/**
 * @brief Get current phase
 * @param vco Pointer to VCO controller structure
 * @return Current total phase in radians
 */
float vco_controller_get_phase(const vco_controller_t* vco);

/**
 * @brief Get current frequency
 * @param vco Pointer to VCO controller structure
 * @return Current total frequency in Hz
 */
float vco_controller_get_frequency(const vco_controller_t* vco);

#endif /* VCO_CONTROLLER_H */