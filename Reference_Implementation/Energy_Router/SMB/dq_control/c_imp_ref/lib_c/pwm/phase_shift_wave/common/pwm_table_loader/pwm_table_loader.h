#ifndef PWM_TABLE_LOADER_H
#define PWM_TABLE_LOADER_H

#include <stdint.h>

typedef struct {
    uint32_t n_init_phase;
    uint32_t n_mod_index;
    uint32_t points_per_cycle;
    float* intersection_table;
    float* pwm_value_table;
    float* ref_point_table;
    float* dt_value_table;
} PWMTables;

typedef struct {
    uint32_t num_points;
    float* pwm_values;
} PWMSingleTable;

PWMTables load_pwm_tables(const char* filename);
void free_pwm_tables(PWMTables* tables);

// Helper to get value from 3D table
float get_table_value(float* table, uint32_t n_init_phase, uint32_t n_mod_index, 
                     uint32_t points_per_cycle, uint32_t phase_idx, 
                     uint32_t mod_idx, uint32_t point_idx);

// Get 1D intersection table for specific modulation index and initial phase
// result_table must be pre-allocated with size points_per_cycle
void get_intersection_1d_table(const PWMTables* tables, float mod_index, float init_phase, float* result_table);

PWMSingleTable load_single_pwm_table(const char* filename);
void free_single_pwm_table(PWMSingleTable* table);

// Get PWM value for a given phase angle using linear interpolation between table points
float get_pwm_value_for_phase(const float* result_table, const float* pwm_values, 
                             uint32_t table_size, float phase_angle);

#endif