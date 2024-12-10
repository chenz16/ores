#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "stair_wave_pwm.h"

bool init_pwm_control(PWMControlState* state) {
    if (!state) {
        fprintf(stderr, "Null state pointer in init_pwm_control\n");
        return false;
    }

    // Initialize basic state variables
    state->update_table = false;
    state->current_angle = 0.0f;
    state->num_modules = NUM_DC_SOURCES;
    state->update_1d_table = true;  // Changed to true to ensure first update
    state->vdc_sort_mode = false;
    state->dc_sources_sort_mode = SORT_BY_PREV_ORDER;
    state->is_initialized = false;
    state->phase_shift = 0.0f;

    // Allocate and initialize dc_sources
    state->dc_sources = (single_dc_source_t*)malloc(state->num_modules * sizeof(single_dc_source_t));
    if (!state->dc_sources) {
        fprintf(stderr, "Failed to allocate dc_sources\n");
        return false;
    }

    // Initialize dc_sources
    for (int i = 0; i < state->num_modules; i++) {
        state->dc_sources[i].vdc = 0.0f;
        state->dc_sources[i].io_index = i;
        state->dc_sources[i].c = 0.0f;
        state->dc_sources[i].m_common = 0.0f;
        state->dc_sources[i].m_self = 0.0f;
        state->dc_sources[i].vdc_ref = 0.0f;
        state->dc_sources[i].phase_shift = 0.0f;
        state->dc_sources[i].soc = 0.0f;
        state->dc_sources[i].pwm_state = STATE_ALL_OFF;
        state->dc_sources[i].valid = true;
    }

    // Initialize pwm_out_state
    state->pwm_out_state = (SwitchingStateResult*)malloc(sizeof(SwitchingStateResult));
    if (!state->pwm_out_state) {
        fprintf(stderr, "Failed to allocate pwm_out_state\n");
        cleanup_pwm_control(state);
        return false;
    }
    state->pwm_out_state->index = 0;
    state->pwm_out_state->state = 0;

    // Initialize angles table
    state->angles_table = load_switching_angles_table_5d();
    if (!state->angles_table) {
        fprintf(stderr, "Failed to load angles table\n");
        free(state->dc_sources);
        state->dc_sources = NULL;
        return false;
    }

    // Initialize stair wave table
    state->table = init_stair_wave_table(state->num_modules);
    if (!state->table) {
        fprintf(stderr, "Failed to init stair wave table\n");
        free_switching_angles_table_5d(state->angles_table);
        free(state->dc_sources);
        state->dc_sources = NULL;
        state->angles_table = NULL;
        return false;
    }

    state->is_initialized = true;
    return true;
}

void cleanup_pwm_control(PWMControlState* state) {
    if (!state) return;

    if (state->table) {
        free_stair_wave_table(state->table);
        state->table = NULL;
    }
    
    if (state->angles_table) {
        free_switching_angles_table_5d(state->angles_table);
        state->angles_table = NULL;
    }

    if (state->dc_sources) {
        free(state->dc_sources);
        state->dc_sources = NULL;
    }

    // Add cleanup for pwm_out_state
    if (state->pwm_out_state) {
        free(state->pwm_out_state);
        state->pwm_out_state = NULL;
    }

    state->is_initialized = false;
}

bool update_pwm_control(PWMControlState* state, single_dc_source_t* current_dc_sources) {
    if (!state || !state->is_initialized || !state->dc_sources || !current_dc_sources || 
        !state->table || !state->angles_table) {
        return false;
    }

    update_dc_sources(
        state->dc_sources,
        current_dc_sources,
        state->dc_sources_sort_mode,
        state->num_modules
    );

    if (state->update_1d_table) {
        update_stair_wave_table(state->table, 
                              state->angles_table, 
                              state->dc_sources, 
                              state->num_modules);
        
        state->phase_shift = state->dc_sources[0].phase_shift;
    }


    get_switching_state(state->table, 
                        state->current_angle + state->phase_shift, 
                        STAIR_WAVE_ANGLE_SAFE_MARGIN, 
                        state->update_table, 
                        state->pwm_out_state);


    for (int i = 0; i < state->num_modules; i++) 
        state->dc_sources[i].pwm_state = state->pwm_out_state->state;
    
    write_ios_from_dc_sources(state->dc_sources, state->num_modules);
    
    return true;
}