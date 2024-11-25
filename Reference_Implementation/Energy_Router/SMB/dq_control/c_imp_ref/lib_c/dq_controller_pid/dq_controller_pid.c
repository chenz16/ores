#include "controller_modelfree.h"

void DQController_Init(DQController_State* state, DQController_Params* params) {
    // Initialize all state variables to zero
    state->id_ref = 0.0f;
    state->iq_ref = 0.0f;
    state->id_meas = 0.0f;
    state->iq_meas = 0.0f;
    state->vd_grid = 0.0f;
    state->vq_grid = 0.0f;
    state->integral_d = 0.0f;
    state->integral_q = 0.0f;
    state->vd_out = 0.0f;
    state->vq_out = 0.0f;

    // Validate integral limits
    if (params->integral_max <= params->integral_min) {
        // Set default values if limits are invalid
        params->integral_max = 1000.0f;  // Example
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
    // Calculate current errors
    float error_d = state->id_ref - state->id_meas;
    float error_q = state->iq_ref - state->iq_meas;

    // Update integral terms with anti-windup
    state->integral_d += error_d * params->ki_d * params->Ts;
    state->integral_q += error_q * params->ki_q * params->Ts;

    // Apply integral limits
    if (state->integral_d > params->integral_max) {
        state->integral_d = params->integral_max;
    } else if (state->integral_d < params->integral_min) {
        state->integral_d = params->integral_min;
    }

    if (state->integral_q > params->integral_max) {
        state->integral_q = params->integral_max;
    } else if (state->integral_q < params->integral_min) {
        state->integral_q = params->integral_min;
    }

    // PI controller outputs
    float vd_pi = params->kp_d * error_d + state->integral_d;
    float vq_pi = params->kp_q * error_q + state->integral_q;

    // Simple feed-forward terms
    // 1. Grid voltage feed-forward for better disturbance rejection
    // 2. Basic cross-coupling compensation
    float vd_ff = state->vd_grid - params->omega * params->Ts * state->iq_meas;
    float vq_ff = state->vq_grid + params->omega * params->Ts * state->id_meas;

    // Combine PI output and feed-forward terms
    state->vd_out = vd_pi + vd_ff;
    state->vq_out = vq_pi + vq_ff;
}

void DQController_SetReference(DQController_State* state, float id_ref, float iq_ref) {
    state->id_ref = id_ref;
    state->iq_ref = iq_ref;
}

void DQController_UpdateMeasurements(DQController_State* state, float id_meas, float iq_meas, 
                                   float vd_grid, float vq_grid) {
    state->id_meas = id_meas;
    state->iq_meas = iq_meas;
    state->vd_grid = vd_grid;
    state->vq_grid = vq_grid;
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