#include "dq_controller_pid.h"

void DQController_Init(DQController_State* state, DQController_Params* params) {
    // Initialize all state variables to zero

    state->id_ref = 0.0f;
    state->iq_ref = 0.0f;
    state->id_meas = 0.0f;
    state->iq_meas = 0.0f;
    state->vd_ref = 0.0f;
    state->vq_ref = 0.0f;
    state->vd_meas = 0.0f;
    state->vq_meas = 0.0f;
    state->integral_d = 0.0f;
    state->integral_q = 0.0f;
    state->vd_out = 0.0f;
    state->vq_out = 0.0f;

    // Validate integral limits
    if (params->integral_max <= params->integral_min) {
        // Set default values if limits are invalid
        params->integral_max = 400.0f;  // Example
    }
}

void DQController_Reset(DQController_State* state) {
    // Reset integral terms and outputs to zero
    state->integral_d = 0.0f;
    state->integral_q = 0.0f;
    state->vd_out = 0.0f;
    state->vq_out = 0.0f;
}

void DQController_Update(DQController_State* state, DQController_Params* params) {
    // D-axis control
    float error_d = state->id_ref - state->id_meas;
    state->integral_d += error_d * params->Ts;
    
    // Anti-windup
    if (state->integral_d > params->integral_max) state->integral_d = params->integral_max;
    if (state->integral_d < params->integral_min) state->integral_d = params->integral_min;
    
    // Calculate feedback and feedforward components separately
    state->vd_fb = (params->kp_d * error_d + params->ki_d * state->integral_d) * CONTROLLER_SIGN;
    state->vd_ff = (params->R * state->id_meas - params->omega * params->L * state->iq_meas ) * (CONTROLLER_SIGN);
    state->vd_ff += state->vd_meas;
    
    state->vd_out = state->vd_fb + state->vd_ff;

    // Q-axis control
    float error_q = state->iq_ref - state->iq_meas;
    state->integral_q += error_q * params->Ts;
    
    // Anti-windup
    if (state->integral_q > params->integral_max) state->integral_q = params->integral_max;
    if (state->integral_q < params->integral_min) state->integral_q = params->integral_min;
    
    // Calculate feedback and feedforward components separately
    state->vq_fb = (params->kp_q * error_q + params->ki_q * state->integral_q) * CONTROLLER_SIGN;
    state->vq_ff = (params->R * state->iq_meas + params->omega * params->L * state->id_meas ) * (CONTROLLER_SIGN);
    state->vq_ff += state->vq_meas;
    state->vq_out = state->vq_fb + state->vq_ff;

    // Add debug prints
    // printf("Debug DQ Controller:\n");
    // printf("vd_fb: %.2f, vd_ff: %.2f, vd_out: %.2f\n", 
    //        state->vd_fb, state->vd_ff, state->vd_out);
    // printf("vq_fb: %.2f, vq_ff: %.2f, vq_out: %.2f\n", 
    //        state->vq_fb, state->vq_ff, state->vq_out);
}

void DQController_SetReference(DQController_State* state, float id_ref, float iq_ref) {
    state->id_ref = id_ref;
    state->iq_ref = iq_ref;
}

void DQController_UpdateMeasurements(DQController_State* state, float id_meas, float iq_meas, 
                                   float vd_grid, float vq_grid) {
    state->id_meas = id_meas;
    state->iq_meas = iq_meas;
    state->vd_meas = vd_grid;
    state->vq_meas = vq_grid;
}

void DQController_SetIntegralTerms(DQController_State* state, float integral_d, float integral_q, DQController_Params* params) {
    // Clamp the provided values within limits
    if (integral_d > params->integral_max) {
        state->integral_d = params->integral_max;
    } else if (integral_d < params->integral_min) {
        state->integral_d = params->integral_min;
    } else {
        state->integral_d = integral_d;
    }

    if (integral_q > params->integral_max) {
        state->integral_q = params->integral_max;
    } else if (integral_q < params->integral_min) {
        state->integral_q = params->integral_min;
    } else {
        state->integral_q = integral_q;
    }
}

float DQController_GetVoltageD(DQController_State* state) {
    return state->vd_out;
}

float DQController_GetVoltageQ(DQController_State* state) {
    return state->vq_out;
}

float DQController_GetVoltageD_FF(DQController_State* state) {
    return state->vd_ff;
}

float DQController_GetVoltageD_FB(DQController_State* state) {
    return state->vd_fb;
}

float DQController_GetVoltageQ_FF(DQController_State* state) {
    return state->vq_ff;
}

float DQController_GetVoltageQ_FB(DQController_State* state) {
    return state->vq_fb;
}
