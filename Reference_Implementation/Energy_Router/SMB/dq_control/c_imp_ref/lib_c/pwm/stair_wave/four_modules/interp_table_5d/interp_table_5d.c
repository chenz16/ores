#include "interp_table_5d.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

// Helper function to find lower index and interpolation factor
static void find_index_and_factor(float value, const float* array, int size, 
                                 int* lower_idx, float* factor) {
    if (!array || !lower_idx || !factor || size <= 1) {
        fprintf(stderr, "Invalid parameters in find_index_and_factor\n");
        return;
    }
    if (value < array[0]) {
        *lower_idx = 0;
        *factor = 0.0f;
        return;
    }
    if (value >= array[size-1]) {
        *lower_idx = size - 2;
        *factor = 1.0f;
        return;
    }

    for (*lower_idx = 0; *lower_idx < size - 1; (*lower_idx)++) {
        if (array[*lower_idx + 1] >= value) {
            break;
        }
    }
    *factor = (value - array[*lower_idx]) / 
              (array[*lower_idx + 1] - array[*lower_idx]);
}

// Helper function to compute V1 magnitude
static float compute_v1(float * theta, single_dc_source_t *dc_sources) {
    if (!theta || !dc_sources) {
        fprintf(stderr, "Null pointer in compute_v1\n");
        return 0.0f;
    }
    
    float sum = 0.0f;
    for (int i = 0; i < NUM_DC_SOURCES; i++) {
        if (!isfinite(theta[i]) || !isfinite(dc_sources[i].c)) {
            fprintf(stderr, "Invalid values in compute_v1: theta[%d]=%.3f, c=%.3f\n", 
                    i, theta[i], dc_sources[i].c);
            return 0.0f;
        }
        sum += dc_sources[i].c * cosf(theta[i]);
    }
    return sum;
}

// Helper function to compute V1 error
static float compute_v1_error(float * theta,
                              single_dc_source_t *dc_sources) {
    float v1_actual = compute_v1(theta, dc_sources);
    float v1_ideal = M_PI * dc_sources[0].m_common;
    return 100.0f * (v1_actual - v1_ideal) / v1_ideal;
}

// Compensate switching angles to minimize V1 error
static void compensate_angles(float* theta,
                              single_dc_source_t *dc_sources,
                              float* final_error) {
    if (!theta || !dc_sources || !final_error) {
        fprintf(stderr, "Null pointer in compensate_angles\n");
        return;
    }

    // Validate input angles
    for (int i = 0; i < NUM_DC_SOURCES; i++) {
        if (!isfinite(theta[i])) {
            fprintf(stderr, "Invalid input angle at index %d\n", i);
            return;
        }
    }

    const float LEARNING_RATE = 0.1f;
    const float ERROR_THRESHOLD = 0.1f;
    const int MAX_ITERATIONS = 7;
    
    float working_theta[NUM_DC_SOURCES];
    memcpy(working_theta, theta, NUM_DC_SOURCES * sizeof(float));
    
    float v1_error = compute_v1_error(working_theta, dc_sources);
    if (!isfinite(v1_error)) {
        fprintf(stderr, "Invalid initial v1_error\n");
        return;
    }

    float v1_ideal = M_PI * dc_sources[0].m_common;
    int iterations = 0;
    
    while (fabsf(v1_error) > ERROR_THRESHOLD && iterations < MAX_ITERATIONS) {
        float v1_actual = compute_v1(working_theta, dc_sources);
        if (!isfinite(v1_actual)) {
            fprintf(stderr, "Invalid v1_actual at iteration %d\n", iterations);
            return;
        }

        float error_factor = 2.0f * (v1_actual - v1_ideal);
        
        for (int i = 0; i < NUM_DC_SOURCES; i++) {
            float gradient = error_factor * (-dc_sources[i].c * sinf(working_theta[i]));
            if (!isfinite(gradient)) {
                fprintf(stderr, "Invalid gradient for angle %d\n", i);
                return;
            }
            working_theta[i] -= LEARNING_RATE * gradient;
            working_theta[i] = fmaxf(0.0f, fminf(M_PI_2, working_theta[i]));
        }
        
        v1_error = compute_v1_error(working_theta, dc_sources);
        iterations++;
    }
    
    // Validate final results before copying back
    for (int i = 0; i < NUM_DC_SOURCES; i++) {
        if (!isfinite(working_theta[i])) {
            fprintf(stderr, "Invalid compensated angle at index %d\n", i);
            return;
        }
    }
    
    memcpy(theta, working_theta, NUM_DC_SOURCES * sizeof(float));
    *final_error = v1_error;
    
    // printf("Debug: Compensation completed after %d iterations\n", iterations);
}

const SwitchingAnglesTable* init_switching_angles_lookup_table(void) {
    // printf("Debug: Starting table initialization...\n");  // Debug output
    
    SwitchingAnglesTable* table = load_switching_angles_table_5d();
    if (!table) {
        fprintf(stderr, "Failed to load switching angles table\n");
        return NULL;
    }

    // Add size validation
    if (table->c1_values == NULL || table->c2_values == NULL || 
        table->c3_values == NULL || table->c4_values == NULL) {
        fprintf(stderr, "Critical: ci_values arrays are NULL\n");
        free_switching_angles_table_5d(table);
        return NULL;
    }

    // Validate array contents
    for (int i = 0; i < N_POINTS_PER_CI; i++) {
        if (!isfinite(table->c1_values[i]) || !isfinite(table->c2_values[i]) ||
            !isfinite(table->c3_values[i]) || !isfinite(table->c4_values[i])) {
            fprintf(stderr, "Invalid ci values at index %d\n", i);
            free_switching_angles_table_5d(table);
            return NULL;
        }
    }

    // Validate angles array
    if (table->angles == NULL) {
        fprintf(stderr, "Critical: angles array is NULL\n");
        free_switching_angles_table_5d(table);
        return NULL;
    }

    printf("Debug: Table initialization successful\n");  // Debug output
    return table;
}

void cleanup_switching_angles_lookup_table(const SwitchingAnglesTable* table) {
    if (!table) {
        fprintf(stderr, "Warning: Attempting to free NULL table\n");
        return;
    }
    
    // Validate table components before freeing
    if (table->angles) {
        printf("Debug: Freeing angles array...\n");
    }
    if (table->c1_values) {
        printf("Debug: Freeing c1_values array...\n");
    }
    // ... similar for other arrays ...
    
    printf("Debug: Starting table cleanup...\n");
    free_switching_angles_table_5d((SwitchingAnglesTable*)table);
    printf("Debug: Cleanup completed\n");
}

void interpolate_switching_angles_5d(
    const SwitchingAnglesTable* table,
    single_dc_source_t* dc_sources,
    SwitchingAnglesResult* result
) {
    static bool first_call = true;
    if (first_call) {
        // printf("Debug: First interpolation call, sizeof(SwitchingAnglesResult)=%zu\n", 
            //    sizeof(SwitchingAnglesResult));
        first_call = false;
    }

    if (!table || !dc_sources || !result) {
        fprintf(stderr, "Null pointer passed to interpolate_switching_angles_5d\n");
        return;
    }

    // Initialize result to safe values FIRST
    memset(result, 0, sizeof(SwitchingAnglesResult));

    // Validate table structure
    if (!table->angles || !table->c1_values || !table->c2_values ||
        !table->c3_values || !table->c4_values || !table->m_values) {
        fprintf(stderr, "Invalid table structure - contains NULL pointers\n");
        return;
    }

    // Validate input values
    if (!isfinite(dc_sources[0].m_common)) {
        fprintf(stderr, "Invalid m_common value\n");
        return;
    }
    
    for (int i = 0; i < NUM_DC_SOURCES; i++) {
        if (!isfinite(dc_sources[i].c)) {
            fprintf(stderr, "Invalid c value at index %d\n", i);
            return;
        }
    }

    // printf("Debug: m_common = %.3f\n", dc_sources[0].m_common);
    // for (int i = 0; i < NUM_DC_SOURCES; i++) {
    //     printf("Debug: dc_source[%d].c = %.3f\n", i, dc_sources[i].c);
    // }

    // Constrain ci values to [0.95, 1.05]
    for (int i = 0; i < NUM_DC_SOURCES; i++) {
        dc_sources[i].c = dc_sources[i].c < 0.95f ? 0.95f : (dc_sources[i].c > 1.05f ? 1.05f : dc_sources[i].c);
    }
    
    // Constrain m to [0.75, 1.0]
    dc_sources[0].m_common = dc_sources[0].m_common < 0.75f ? 0.75f : (dc_sources[0].m_common > 1.0f ? 1.0f : dc_sources[0].m_common);

    // Find indices and factors for ci values
    int ci_idx[NUM_DC_SOURCES];
    float ci_factor[NUM_DC_SOURCES];
    for (int i = 0; i < NUM_DC_SOURCES; i++) {
        find_index_and_factor(dc_sources[i].c, table->c1_values, N_POINTS_PER_CI, &ci_idx[i], &ci_factor[i]);
    }
    
    // Find index and factor for modulation index
    int m_idx;
    float m_factor;
    find_index_and_factor(dc_sources[0].m_common, table->m_values, N_MOD_INDEX, &m_idx, &m_factor);

    // Interpolate angles for each DC source
    for (int dc = 0; dc < NUM_DC_SOURCES; dc++) {
        if (ci_idx[0] >= N_POINTS_PER_CI-1 || ci_idx[1] >= N_POINTS_PER_CI-1 || 
            ci_idx[2] >= N_POINTS_PER_CI-1 || ci_idx[3] >= N_POINTS_PER_CI-1 || 
            m_idx >= N_MOD_INDEX-1) {
            fprintf(stderr, "Index out of bounds in interpolation\n");
            return;
        }
        // Get base angle
        float base_angle = table->angles[ci_idx[0]][ci_idx[1]][ci_idx[2]][ci_idx[3]][m_idx][dc];
        
        // First-order approximation (Taylor expansion)
        float delta_angle = 0.0f;
        
        // Add contribution from each ci dimension
        for (int i = 0; i < NUM_DC_SOURCES; i++) {
            float next_angle = table->angles[ci_idx[0] + (i==0)][ci_idx[1] + (i==1)][ci_idx[2] + (i==2)][ci_idx[3] + (i==3)][m_idx][dc];
            delta_angle += (next_angle - base_angle) * ci_factor[i];
        }
        
        // Add contribution from modulation index
        float next_m_angle = table->angles[ci_idx[0]][ci_idx[1]][ci_idx[2]][ci_idx[3]][m_idx + 1][dc];
        delta_angle += (next_m_angle - base_angle) * m_factor;
        
        result->theta[dc] = base_angle + delta_angle;
        result->theta[dc] = fmaxf(0.0f, fminf(M_PI_2, result->theta[dc]));
    }
    
    // After interpolation, validate results before compensation
    // printf("Debug: Interpolated angles before compensation:\n");
    for (int i = 0; i < NUM_DC_SOURCES; i++) {
        // printf("Debug: theta[%d] = %.3f\n", i, result->theta[i]);
        if (!isfinite(result->theta[i])) {
            fprintf(stderr, "Invalid interpolated angle at index %d\n", i);
            memset(result->theta, 0, NUM_DC_SOURCES * sizeof(float));
            result->v1_error = 0.0f;
            return;
        }
    }
    
    // Update the call to compensate_angles with additional debug
    float temp_error = 0.0f;
    // printf("Debug: Starting angle compensation...\n");
    compensate_angles(result->theta, dc_sources, &temp_error);
    // printf("Debug: Compensation completed with error: %.3f\n", temp_error);
    
    // Update final V1 error
    result->v1_error = temp_error;

    // After compensation, ensure results are valid
    // printf("Debug: Final validation of results...\n");
    bool valid_results = true;
    for (int i = 0; i < NUM_DC_SOURCES; i++) {
        if (!isfinite(result->theta[i])) {
            valid_results = false;
            fprintf(stderr, "Invalid final angle at index %d\n", i);
            break;
        }
        // Ensure angles are within valid range
        if (result->theta[i] < 0.0f || result->theta[i] > M_PI_2) {
            valid_results = false;
            fprintf(stderr, "Angle out of range at index %d: %.3f\n", i, result->theta[i]);
            break;
        }
    }

    if (!valid_results) {
        memset(result->theta, 0, NUM_DC_SOURCES * sizeof(float));
        result->v1_error = 0.0f;
        fprintf(stderr, "Results invalidated, reset to zero\n");
    }

    // Print final values for debugging
    // printf("Debug: Final angles: [%.3f, %.3f, %.3f, %.3f]\n",
    //        result->theta[0], result->theta[1], 
    //        result->theta[2], result->theta[3]);
    // printf("Debug: Final v1_error: %.3f\n", result->v1_error);
    // printf("Debug: Results valid: %d\n", valid_results);
}

// Helper function to safely copy results
static void copy_result(SwitchingAnglesResult* dest, const SwitchingAnglesResult* src) {
    if (!dest || !src) {
        fprintf(stderr, "Null pointer in copy_result\n");
        return;
    }
    memcpy(dest->theta, src->theta, NUM_DC_SOURCES * sizeof(float));
    dest->v1_error = src->v1_error;
}

void interpolate_switching_angles_5d_with_correction(
    const SwitchingAnglesTable* table,
    single_dc_source_t* dc_sources,
    SwitchingAnglesResult* result
) {
    if (!table || !dc_sources || !result) return;

    // Initialize result
    memset(result, 0, sizeof(SwitchingAnglesResult));

    // Do normal lookup
    interpolate_switching_angles_5d(table, dc_sources, result);
    
    // Store backup
    float backup_angles[NUM_DC_SOURCES];
    memcpy(backup_angles, result->theta, NUM_DC_SOURCES * sizeof(float));
    
    // Apply correction
    float final_error = 0.0f;
    compensate_angles(result->theta, dc_sources, &final_error);
    
    // Validate results
    bool valid = true;
    for (int i = 0; i < NUM_DC_SOURCES; i++) {
        if (!isfinite(result->theta[i])) {
            valid = false;
            break;
        }
    }
    
    if (!valid) {
        memcpy(result->theta, backup_angles, NUM_DC_SOURCES * sizeof(float));
        result->v1_error = compute_v1_error(backup_angles, dc_sources);
    } else {
        result->v1_error = final_error;
    }
} 