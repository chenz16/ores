#include "phase_shift_xmodule.h"
#include <stdlib.h>
#include <float.h>
#include <math.h>

typedef struct {
    float current_phase;
    float init_phase;
    uint32_t current_index;
    int8_t current_status;  // -1, 0, or 1
} ModuleState;

MMCPWMStatus get_mmc_pwm_status(const PWMTables* tables, 
                               const float* module_init_phases,
                               uint32_t num_modules,
                               float mod_index,
                               float current_phase,
                               float* next_interrupt_time) {
    MMCPWMStatus status = {
        .module_status = malloc(num_modules * sizeof(int8_t)),
        .num_modules = num_modules
    };
    
    if (!status.module_status) {
        status.num_modules = 0;
        *next_interrupt_time = 0;
        return status;
    }

    // Allocate temporary storage for module states
    ModuleState* modules = malloc(num_modules * sizeof(ModuleState));
    if (!modules) {
        free(status.module_status);
        status.module_status = NULL;
        status.num_modules = 0;
        *next_interrupt_time = 0;
        return status;
    }

    // Allocate temporary storage for intersection tables
    float* intersection_table = malloc(tables->points_per_cycle * sizeof(float));
    if (!intersection_table) {
        free(modules);
        free(status.module_status);
        status.module_status = NULL;
        status.num_modules = 0;
        *next_interrupt_time = 0;
        return status;
    }

    *next_interrupt_time = FLT_MAX;

    // Process each module
    for (uint32_t i = 0; i < num_modules; i++) {
        // Get intersection table for this module's initial phase
        get_intersection_1d_table(tables, mod_index, module_init_phases[i], intersection_table);

        // Calculate effective phase for this module
        float effective_phase = fmod(current_phase + module_init_phases[i], 2 * M_PI);
        if (effective_phase < 0) {
            effective_phase += 2 * M_PI;
        }

        // Find current position in intersection table
        uint32_t current_idx = 0;
        while (current_idx < tables->points_per_cycle - 1 && 
               intersection_table[current_idx] <= effective_phase) {
            current_idx++;
        }
        if (current_idx > 0) current_idx--;

        // Get PWM value for current position
        float pwm_value = get_table_value(tables->pwm_value_table, 
                                        tables->n_init_phase,
                                        tables->n_mod_index,
                                        tables->points_per_cycle,
                                        i % tables->n_init_phase,  // Wrap around if needed
                                        (uint32_t)(mod_index * (tables->n_mod_index - 1)),
                                        current_idx);

        // Convert PWM value to status (-1, 0, 1)
        status.module_status[i] = (pwm_value > 0.5) ? 1 : ((pwm_value < -0.5) ? -1 : 0);

        // Calculate time to next intersection
        float time_to_next = intersection_table[current_idx + 1] - effective_phase;
        if (time_to_next < 0) {
            time_to_next += 2 * M_PI;  // Wrap around to next cycle
        }

        // Update minimum interrupt time
        if (time_to_next < *next_interrupt_time) {
            *next_interrupt_time = time_to_next;
        }
    }

    // Cleanup
    free(intersection_table);
    free(modules);

    return status;
}

void free_mmc_pwm_status(MMCPWMStatus* status) {
    if (status) {
        free(status->module_status);
        status->module_status = NULL;
        status->num_modules = 0;
    }
} 