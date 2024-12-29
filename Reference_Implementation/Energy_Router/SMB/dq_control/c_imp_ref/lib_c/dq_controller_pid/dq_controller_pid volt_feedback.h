#ifndef DQ_CONTROLLER_PID_VOLT_FEEDBACK_H
#define DQ_CONTROLLER_PID_VOLT_FEEDBACK_H

#include "dq_controller_pid.h"

typedef struct {
    float kp_d;
    float ki_d;
    float kp_q;
    float ki_q;
    float Ts;
    float integral_max;
    float integral_min;
} DQControllerVoltFeedback_Params;

typedef DQController_State DQControllerVoltFeedback_State;

// typedef struct {

//     float vd_ref;
//     float vq_ref;

//     float vd_meas;
//     float vq_meas;

//     float integral_d;
//     float integral_q;
//     float vd_out;
//     float vq_out;
//     float vd_fb;
//     float vq_fb;
//     float vd_ff;
//     float vq_ff;
// } DQControllerVoltFeedback_State;

// Function prototypes
void DQControllerVoltFeedback_Init(DQControllerVoltFeedback_State* state, DQControllerVoltFeedback_Params* params);
void DQControllerVoltFeedback_Reset(DQControllerVoltFeedback_State* state);
void DQControllerVoltFeedback_Update(DQControllerVoltFeedback_State* state, DQControllerVoltFeedback_Params* params);
void DQControllerVoltFeedback_SetIntegralTerms(DQControllerVoltFeedback_State* state, float integral_d, float integral_q, DQControllerVoltFeedback_Params* params);

// Utility functions
void DQControllerVoltFeedback_SetReference(DQControllerVoltFeedback_State* state, float vd_ref, float vq_ref);
void DQControllerVoltFeedback_UpdateMeasurements(DQControllerVoltFeedback_State* state, float vd_meas, float vq_meas);

// Getter functions
float DQControllerVoltFeedback_GetVoltageD(DQControllerVoltFeedback_State* state);
float DQControllerVoltFeedback_GetVoltageQ(DQControllerVoltFeedback_State* state);
float DQControllerVoltFeedback_GetVoltageD_FF(DQControllerVoltFeedback_State* state);
float DQControllerVoltFeedback_GetVoltageD_FB(DQControllerVoltFeedback_State* state);
float DQControllerVoltFeedback_GetVoltageQ_FF(DQControllerVoltFeedback_State* state);
float DQControllerVoltFeedback_GetVoltageQ_FB(DQControllerVoltFeedback_State* state);

#endif // DQ_CONTROLLER_PID_VOLT_FEEDBACK_H
