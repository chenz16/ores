#ifndef PLL_CONTROLLER_PI_H
#define PLL_CONTROLLER_PI_H

#include <stdio.h>

// Error codes
typedef enum {
    PI_ERROR_NONE = 0,
    PI_ERROR_NULL_POINTER,
    PI_ERROR_INVALID_PARAMETER
} pi_error_t;

// PI controller structure
typedef struct {
    float kp;                // Proportional gain
    float ki;                // Integral gain
    float ts;                // Sampling time
    float integrator;        // Integral term
    float proportional;      // Proportional term
    float integrator_max;    // Maximum integrator value
    float integrator_min;    // Minimum integrator value
    float last_error;        // Previous error
    float control_signal;    // Output control signal
} pi_controller_t;

// Function declarations
pi_error_t pi_controller_init(pi_controller_t* pi, 
                             float kp, 
                             float ki, 
                             float ts,
                             float integrator_max,
                             float integrator_min);

pi_error_t pi_controller_update(pi_controller_t* pi, float error);

pi_error_t pi_controller_reset(pi_controller_t* pi);

float pi_controller_get_output(const pi_controller_t* pi);

float pi_controller_get_proportional(const pi_controller_t* pi);

float pi_controller_get_integrator(const pi_controller_t* pi);

#endif // PLL_CONTROLLER_PI_H