#ifndef STAIR_WAVE_PWM_H
#define STAIR_WAVE_PWM_H

#include <stdbool.h>
#include "../load_table_5d/load_switching_angles_table_5d.h"
#include "../lookup_table_1d/dyn_stair_wave_table.h"
#include "../../common/table_def/table_def.h"
#include "../../common/write_ios/write_ios.h"
#include "../../common/modulation_2pwm/modulation_2pwm.h"


typedef struct {
    StairWaveTable* table;              // Dynamic 1D lookup table
    SwitchingAnglesTable* angles_table; // Base 5D lookup table
    single_dc_source_t* dc_sources;     // Array of DC sources
    int num_modules;                    // Number of modules
    float current_angle;                // Current angle
    bool update_table;                  // Update table flag when feedback occurs
    bool reset_dc_sources;              // Reset DC sources flag
    uint8_t dc_sources_sort_mode;      // Sort mode
    bool is_initialized;                // Initialization flag
    bool vdc_sort_mode;
    bool update_1d_table;
    float phase_shift;
} PWMControlState;


bool init_pwm_control(PWMControlState* state);
bool update_pwm_control( PWMControlState* state, 
                        single_dc_source_t * current_dc_sources);

void cleanup_pwm_control(PWMControlState* state);

#endif // STAIR_WAVE_PWM_H
