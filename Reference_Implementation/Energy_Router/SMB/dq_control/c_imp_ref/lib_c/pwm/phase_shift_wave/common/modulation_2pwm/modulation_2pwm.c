#include "modulation_2pwm.h"
#include <stddef.h>
#include <math.h>
#include <string.h>

// Forward declarations of static functions
static void swap_sources(single_dc_source_t *a, single_dc_source_t *b);
static int partition(single_dc_source_t *dc_sources, int low, int high, float soc_threshold);
static void quicksort(single_dc_source_t *dc_sources, int low, int high, float soc_threshold);

void init_dc_sources(
    single_dc_source_t * dc_sources,
    const int num_of_modules
) {
    if (!dc_sources || num_of_modules <= 0 || num_of_modules > MAX_NUM_MODULES) {
        return;
    }

    for (int i = 0; i < num_of_modules; i++) {
        dc_sources[i].vdc = 0.0f;
        dc_sources[i].io_index = i;
        dc_sources[i].c = 0.0f;
        dc_sources[i].m_common = 0.0f;
        dc_sources[i].m_self = 0.0f;
        dc_sources[i].vdc_ref = 0.0f;
        dc_sources[i].phase_shift = 0.0f;
        dc_sources[i].soc = 0.8f;
        dc_sources[i].pwm_state = STATE_ALL_OFF;
        dc_sources[i].valid = false;
    }
}

uint8_t optimize_sort_strategy_based_on_interruption_mode(uint8_t interruption_mode) 
{
    if(interruption_mode == INTERRUPTION_MODE_TIMMER_REQ)
        return SORT_BY_PREV_ORDER;

    else if(interruption_mode == INTERRUPTION_MODE_FEEDBACK_REQ || interruption_mode == INTERRUPTION_MODE_ZERO_CROSS)
        return SORT_BY_VDC_HI;

    else if(interruption_mode == INTERRUPTION_MODE_SOC_BALANCE)
        return SORT_BY_SOC_BALANCE;

    else
        return SORT_BY_PREV_ROTATION;
}

void update_dc_sources(
    single_dc_source_t * dc_sources,
    single_dc_source_t * new_dc_sources,
    uint8_t sort_mode, 
    const int num_of_modules
) {
    if (!dc_sources || !new_dc_sources) return;
    // Copy back to original array while preserving io_index mapping
    if(sort_mode == SORT_BY_SOC_BALANCE) {
        * dc_sources = *new_dc_sources;
        sort_dc_sources(dc_sources, num_of_modules, 0.05f);
    }
    else if(sort_mode == SORT_BY_VDC_HI) {
        * dc_sources = * new_dc_sources;
        sort_dc_sources(dc_sources, num_of_modules, 0.0f);
    }
    else if (sort_mode == SORT_BY_PREV_ORDER) { 
        for (int i = 0; i < num_of_modules; i++) {
            dc_sources[i] = new_dc_sources[dc_sources[i].io_index];
        }
    }
    else if (sort_mode == SORT_BY_PREV_ROTATION) {
        for (int i = 0; i < num_of_modules - 1; i++) {
            uint8_t index = (dc_sources[i].io_index + 1) % num_of_modules;
            dc_sources[i] = new_dc_sources[index];
            dc_sources[i].io_index = index;
        }
    }
    else {
        *dc_sources = * new_dc_sources;
     }

}

static void swap_sources(single_dc_source_t *a, single_dc_source_t *b) {
    single_dc_source_t temp = *a;
    *a = *b;
    *b = temp;
}

static int partition(single_dc_source_t *dc_sources, int low, int high, float soc_threshold) {
    single_dc_source_t pivot = dc_sources[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        bool should_swap = false;
        float soc_diff = dc_sources[j].soc - pivot.soc;
        
        if (fabsf(soc_diff) > soc_threshold) {
            should_swap = (soc_diff > 0);  // Higher SOC first
        } else {
            should_swap = (dc_sources[j].vdc > pivot.vdc);  // Higher voltage first
        }
        
        if (should_swap) {
            i++;
            swap_sources(&dc_sources[i], &dc_sources[j]);
        }
    }
    swap_sources(&dc_sources[i + 1], &dc_sources[high]);
    return i + 1;
}

static void quicksort(single_dc_source_t *dc_sources, int low, int high, float soc_threshold) {
    if (low < high) {
        int pi = partition(dc_sources, low, high, soc_threshold);
        quicksort(dc_sources, low, pi - 1, soc_threshold);
        quicksort(dc_sources, pi + 1, high, soc_threshold);
    }
}

void sort_dc_sources(
    single_dc_source_t *dc_sources,
    int num_of_modules,
    float soc_threshold
) {
    if (!dc_sources || num_of_modules <= 0 || num_of_modules > MAX_NUM_MODULES) {
        return;
    }
    
    quicksort(dc_sources, 0, num_of_modules - 1, soc_threshold);
}

void convert_system_state_to_module_state(
    single_dc_source_t * sorted_dc_sources,
    SwitchingStateResult * switching_sum,
    int num_of_modules
) {
    // First, set all modules to bypass state
    for (int i = 0; i < num_of_modules; i++) {
        sorted_dc_sources[i].pwm_state = STATE_BYPASS;
    }

    if (switching_sum->state > 0) {
        // For positive states, set that many modules to positive
        for (int i = 0; i < switching_sum->state && i < num_of_modules; i++) {
            if (sorted_dc_sources[i].valid) {
                sorted_dc_sources[i].pwm_state = STATE_POSITIVE;
            }
        }
    } else if (switching_sum->state < 0) {
        // For negative states, set that many modules to negative
        for (int i = 0; i < -switching_sum->state && i < num_of_modules; i++) {
            if (sorted_dc_sources[i].valid) {
                sorted_dc_sources[i].pwm_state = STATE_NEGATIVE;
            }
        }
    }
}

uint32_t convert_HBridgeState_to_binary(HBridgeState state) {
    uint32_t output_value = 0;
    
    // Format: [S1H S1L S2H S2L S3H S3L S4H S4L]
    switch (state) {
        case STATE_POSITIVE:
            output_value = 0b10100000;  // High-side switches ON for output
            break;
        case STATE_NEGATIVE:
            output_value = 0b01010000;  // Low-side switches ON for output
            break;
        case STATE_BYPASS_LOW:
            output_value = 0b00110000;  // Low-side bypass
            break;
        case STATE_BYPASS_HIGH:
            output_value = 0b11000000;  // High-side bypass
            break;
        case STATE_BYPASS:
            output_value = 0b11000000;  // Same as HIGH bypass
            break;
        case STATE_ALL_OFF:
        default:
            output_value = 0b00000000;  // All switches OFF (safe state)
            break;
    }
    return output_value;
}


