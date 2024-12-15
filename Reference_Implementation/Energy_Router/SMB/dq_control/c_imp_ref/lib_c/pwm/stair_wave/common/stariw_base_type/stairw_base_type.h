#ifndef STAIRW_BASE_TYPE_H
#define STAIRW_BASE_TYPE_H


#include <stdint.h>
#include <stdbool.h>

// 1D switching table [angle, value]
#define NUM_DC_SOURCES 4
#define NUM_1D_TABLE_POINTS 18
#define STAIR_WAVE_ANGLE_SAFE_MARGIN 0.00001f

#define SORT_BY_PREV_ORDER 0    // based on previous order
#define SORT_BY_CURRENT_ORDER 1 // based on the seq of array 
#define SORT_BY_SOC_BALANCE 2   // based on soc balance
#define SORT_BY_VDC_HI 3         // based on vdc highest
#define SORT_BY_PREV_ROTATION 4  // based on previous rotation

#define INTERRUPTION_MODE_TIMMER_REQ 0
#define INTERRUPTION_MODE_FEEDBACK_REQ 1
#define INTERRUPTION_MODE_ZERO_CROSS 2
#define INTERRUPTION_MODE_SOC_BALANCE 3

typedef enum {
    STATE_ALL_OFF = 0,
    STATE_POSITIVE = 1,
    STATE_NEGATIVE = -1,
    STATE_BYPASS_LOW = 2,
    STATE_BYPASS_HIGH = -2,
    STATE_BYPASS = 3
} HBridgeState;
// Given the angle, return the state and index
typedef struct {
    int state; // 1d table sum_value
    int index; // 1d table index
    float phase_2next; // used for timer remaining time for next int
} SwitchingStateResult;
typedef SwitchingStateResult PWMOutState;


// stair wave table
typedef struct {
    float* angles;
    int*  sum_index;
    float * sum_value;
    int num_points;
} DynamicTable1D;
typedef DynamicTable1D StairWaveTable;

// (total) lookup results for switching angles
typedef struct {
    float theta[NUM_DC_SOURCES];
    float  thd;
    float v1_error;
} SwitchingAnglesResult;
typedef SwitchingAnglesResult SwitchingAnglesRawLookup;

// single dc source
typedef struct 
{
    float vdc;
    uint8_t io_index;
    uint8_t modulation_seq;
    float c;
    float m_common;
    float m_self;
    float vdc_ref;
    float phase_shift;
    float soc;
    HBridgeState pwm_state;
    bool valid;
} single_dc_source_t;

typedef struct {
    float vdc1;
    float vdc2;
    float vdc3;
    float vdc4;
    uint8_t io_index1;
    uint8_t io_index2;
    uint8_t io_index3;
    uint8_t io_index4;
    float phase_shift;
    float modulation_index;
    uint8_t sort_strategy;
    uint8_t num_modules;
    float pwm_state_phase_safe_margine;
    uint8_t interruption_mode; // 0:
}pwm_mini_input_t;



#endif