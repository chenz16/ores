#ifndef MODULATION_2PWM_H
#define MODULATION_2PWM_H

#include "../../../../dq_to_modulation/dq_to_modulation.h"
#include <stdbool.h>
#include <stdint.h>
#include "../../four_modules/lookup_table_1d/dyn_stair_wave_table.h"
#include "../../four_modules/interp_table_5d/interp_table_5d.h"
#include "../write_ios/write_ios.h"
#include "../table_def/table_def.h"
#include <math.h>


#define MAX_NUM_MODULES 4



#include "../write_ios/write_ios.h"

void init_dc_sources(
    single_dc_source_t * dc_sources,
    const int num_of_modules
);


void update_dc_sources(
    single_dc_source_t * dc_sources,
    single_dc_source_t * new_dc_sources,
    uint8_t sort_mode, 
    const int num_of_modules
);

void sort_dc_sources(
    single_dc_source_t * dc_sources,
    int num_of_modules,
    float soc_threshold
);

void convert_system_state_to_module_state(
    single_dc_source_t *sorted_dc_sources,
    SwitchingStateResult * switching_sum,
    int num_of_modules
);

uint8_t optimize_sort_strategy_based_on_interruption_mode(uint8_t interruption_mode);

uint32_t convert_HBridgeState_to_binary(HBridgeState state);

#endif // MODULATION_2PWM_H