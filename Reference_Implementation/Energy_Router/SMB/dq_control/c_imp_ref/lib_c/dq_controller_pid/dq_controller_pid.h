#ifndef DQ_CONTROLLER_PID_H
#define DQ_CONTROLLER_PID_H

#include <stdint.h>

// Controller parameters structure
typedef struct {
    float kp_d;          // Proportional gain for d-axis
    float ki_d;          // Integral gain for d-axis
    float kp_q;          // Proportional gain for q-axis
    float ki_q;          // Integral gain for q-axis
    float omega;         // Grid frequency in rad/s
    float Ts;            // Sampling time
    float integral_max;   // Maximum integral term value
    float integral_min;   // Minimum integral term value
    float R;             // Estimated filter resistance [Ohm]
    float L;             // Estimated filter inductance [H]
} DQController_Params;

// Controller state structure
typedef struct {
    float id_ref;        // Reference d-axis current
    float iq_ref;        // Reference q-axis current
    float id_meas;       // Measured d-axis current
    float iq_meas;       // Measured q-axis current
    float vd_grid;       // Grid voltage d component
    float vq_grid;       // Grid voltage q component
    float integral_d;    // Integral term for d-axis
    float integral_q;    // Integral term for q-axis
    float vd_out;        // Output voltage d component
    float vq_out;        // Output voltage q component
    float vd_ff;         // Feedforward voltage d component
    float vd_fb;         // Feedback voltage d component
    float vq_ff;         // Feedforward voltage q component
    float vq_fb;         // Feedback voltage q component
} DQController_State;

// Function prototypes
void DQController_Init(DQController_State* state, DQController_Params* params);
void DQController_Reset(DQController_State* state);
void DQController_Update(DQController_State* state, DQController_Params* params);
void DQController_SetIntegralTerms(DQController_State* state, float integral_d, float integral_q, DQController_Params* params);

// Utility functions
void DQController_SetReference(DQController_State* state, float id_ref, float iq_ref);
void DQController_UpdateMeasurements(DQController_State* state, float id_meas, float iq_meas, 
                                   float vd_grid, float vq_grid);

// Getter functions
float DQController_GetVoltageD(DQController_State* state);
float DQController_GetVoltageQ(DQController_State* state);
float DQController_GetVoltageD_FF(DQController_State* state);
float DQController_GetVoltageD_FB(DQController_State* state);
float DQController_GetVoltageQ_FF(DQController_State* state);
float DQController_GetVoltageQ_FB(DQController_State* state);

#endif /* DQ_CONTROLLER_PID_H */