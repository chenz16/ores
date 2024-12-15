#include "pwm_table_loader.h"
#include <stdio.h>
#include <stdlib.h>

float get_table_value(float* table, uint32_t n_init_phase, uint32_t n_mod_index, 
                     uint32_t points_per_cycle, uint32_t phase_idx, 
                     uint32_t mod_idx, uint32_t point_idx) {
    // Calculate 3D array index: table[phase_idx][mod_idx][point_idx]
    uint32_t index = (phase_idx * n_mod_index * points_per_cycle) + 
                    (mod_idx * points_per_cycle) + 
                    point_idx;
    return table[index];
}

PWMTables load_pwm_tables(const char* filename) {
    PWMTables tables = {0};
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return tables;
    }

    // Read dimensions
    fread(&tables.n_init_phase, sizeof(uint32_t), 1, file);
    fread(&tables.n_mod_index, sizeof(uint32_t), 1, file);
    fread(&tables.points_per_cycle, sizeof(uint32_t), 1, file);

    // Calculate total size for each table
    size_t table_size = tables.n_init_phase * tables.n_mod_index * tables.points_per_cycle;

    // Allocate memory for tables
    tables.intersection_table = (float*)malloc(table_size * sizeof(float));
    tables.pwm_value_table = (float*)malloc(table_size * sizeof(float));
    tables.ref_point_table = (float*)malloc(table_size * sizeof(float));
    tables.dt_value_table = (float*)malloc(table_size * sizeof(float));

    // Read table data
    fread(tables.intersection_table, sizeof(float), table_size, file);
    fread(tables.pwm_value_table, sizeof(float), table_size, file);
    fread(tables.ref_point_table, sizeof(float), table_size, file);
    fread(tables.dt_value_table, sizeof(float), table_size, file);

    fclose(file);
    return tables;
}

void free_pwm_tables(PWMTables* tables) {
    if (tables) {
        free(tables->intersection_table);
        free(tables->pwm_value_table);
        free(tables->ref_point_table);
        free(tables->dt_value_table);
        
        tables->intersection_table = NULL;
        tables->pwm_value_table = NULL;
        tables->ref_point_table = NULL;
        tables->dt_value_table = NULL;
    }
}

void get_intersection_1d_table(const PWMTables* tables, float mod_index, float init_phase, float* result_table) {
    // Find the bracketing indices for modulation index
    uint32_t mod_idx_low = 0;
    while (mod_idx_low < tables->n_mod_index - 1 && 
           (float)mod_idx_low / (tables->n_mod_index - 1) < mod_index) {
        mod_idx_low++;
    }
    if (mod_idx_low > 0) mod_idx_low--;
    uint32_t mod_idx_high = mod_idx_low + 1;
    if (mod_idx_high >= tables->n_mod_index) mod_idx_high = tables->n_mod_index - 1;

    // Find the bracketing indices for initial phase
    uint32_t phase_idx_low = 0;
    while (phase_idx_low < tables->n_init_phase - 1 && 
           (float)phase_idx_low / (tables->n_init_phase - 1) * 2 * M_PI < init_phase) {
        phase_idx_low++;
    }
    if (phase_idx_low > 0) phase_idx_low--;
    uint32_t phase_idx_high = phase_idx_low + 1;
    if (phase_idx_high >= tables->n_init_phase) phase_idx_high = tables->n_init_phase - 1;

    // Calculate interpolation factors
    float mod_frac = (mod_index * (tables->n_mod_index - 1)) - mod_idx_low;
    float phase_frac = (init_phase * (tables->n_init_phase - 1) / (2 * M_PI)) - phase_idx_low;

    // For each point in the cycle
    for (uint32_t i = 0; i < tables->points_per_cycle; i++) {
        // Get the four corner values
        float v00 = get_table_value(tables->intersection_table, tables->n_init_phase, 
                                  tables->n_mod_index, tables->points_per_cycle,
                                  phase_idx_low, mod_idx_low, i);
        float v01 = get_table_value(tables->intersection_table, tables->n_init_phase, 
                                  tables->n_mod_index, tables->points_per_cycle,
                                  phase_idx_low, mod_idx_high, i);
        float v10 = get_table_value(tables->intersection_table, tables->n_init_phase, 
                                  tables->n_mod_index, tables->points_per_cycle,
                                  phase_idx_high, mod_idx_low, i);
        float v11 = get_table_value(tables->intersection_table, tables->n_init_phase, 
                                  tables->n_mod_index, tables->points_per_cycle,
                                  phase_idx_high, mod_idx_high, i);

        // Bilinear interpolation
        float tmp0 = v00 * (1 - mod_frac) + v01 * mod_frac;
        float tmp1 = v10 * (1 - mod_frac) + v11 * mod_frac;
        result_table[i] = tmp0 * (1 - phase_frac) + tmp1 * phase_frac;
    }
}

PWMSingleTable load_single_pwm_table(const char* filename) {
    PWMSingleTable table = {0};
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return table;
    }

    // Read number of points
    fread(&table.num_points, sizeof(uint32_t), 1, file);

    // Allocate memory for PWM values
    table.pwm_values = (float*)malloc(table.num_points * sizeof(float));
    if (table.pwm_values) {
        // Read PWM values
        fread(table.pwm_values, sizeof(float), table.num_points, file);
    }

    fclose(file);
    return table;
}

void free_single_pwm_table(PWMSingleTable* table) {
    if (table) {
        free(table->pwm_values);
        table->pwm_values = NULL;
        table->num_points = 0;
    }
}