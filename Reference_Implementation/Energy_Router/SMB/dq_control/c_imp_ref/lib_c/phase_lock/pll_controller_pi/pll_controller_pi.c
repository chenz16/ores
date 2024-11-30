#include <stdio.h>
#include "pll_controller_pi.h"

pi_error_t pi_controller_init(pi_controller_t* pi, 
                             float kp, 
                             float ki, 
                             float ts,
                             float integrator_max,
                             float integrator_min) {
    if (!pi) {
        return PI_ERROR_NULL_POINTER;
    }
    
    // Validate integrator limits
    if (integrator_max <= integrator_min) {
        return PI_ERROR_INVALID_PARAMETER;
    }
    
    pi->kp = kp;
    pi->ki = ki;
    pi->ts = ts;
    pi->integrator_max = integrator_max;
    pi->integrator_min = integrator_min;
    
    // Initialize states
    pi->integrator = 0.0f;
    pi->proportional = 0.0f;
    pi->last_error = 0.0f;
    pi->control_signal = 0.0f;
    
    return PI_ERROR_NONE;
}

pi_error_t pi_controller_update(pi_controller_t* pi, float error) {
    if (!pi) return PI_ERROR_NULL_POINTER;

    pi->proportional = pi->kp * error;    
    pi->integrator += pi->ki * error * pi->ts;
    pi->control_signal = pi->proportional + pi->integrator;
    
    return PI_ERROR_NONE;
}

pi_error_t pi_controller_reset(pi_controller_t* pi) {
    if (!pi) {
        return PI_ERROR_NULL_POINTER;
    }
    
    pi->integrator = 0.0f;
    pi->last_error = 0.0f;
    pi->control_signal = 0.0f;
    
    return PI_ERROR_NONE;
}

float pi_controller_get_output(const pi_controller_t* pi) {
    if (pi == NULL) {
        return 0.0f;
    }
    return pi->control_signal;
}

float pi_controller_get_proportional(const pi_controller_t* pi) {
    return pi->proportional;
}

float pi_controller_get_integrator(const pi_controller_t* pi) {
    return pi->integrator;
}