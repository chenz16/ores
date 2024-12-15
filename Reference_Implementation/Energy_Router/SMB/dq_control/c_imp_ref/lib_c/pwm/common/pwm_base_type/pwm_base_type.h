
#ifndef PWM_BASE_TYPE_H
#define PWM_BASE_TYPE_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    STATE_ALL_OFF = 0,
    STATE_POSITIVE = 1,
    STATE_NEGATIVE = -1,
    STATE_BYPASS_LOW = 2,
    STATE_BYPASS_HIGH = -2,
    STATE_BYPASS = 3
} HBridgeState;

typedef struct 
{
    HBridgeState pwm_state;
    float vdc;
    float vdc_ref; // average vdc
    const uint8_t io_index; // io pin index
    float c;     // = vdc/vdc_avg
    float m_common; // common modulation index
    float m_self;  // self modulation index
    float phase_shift; // group realtive phase shift to ref d axis
    float soc; // state of charge
    float soc_avg; // average state of charge
    bool valid; // if this module is valid
} single_dc_source_t;

typedef struct {
    const uint8_t num_modules;
    float   * vdc;
    uint8_t * io_index;
    float * soc;
    float phase_shift;
    float modulation_index;
    uint8_t blance_strategy;
    float pwm_state_phase_safe_margine;
    uint8_t interruption_mode; // 0:
}pwm_mini_input_t;

#endif // BASE_TYPE_H