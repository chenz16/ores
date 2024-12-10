#include "dyn_stair_wave_table.h"
#include "../interp_table_5d/interp_table_5d.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

StairWaveTable* init_stair_wave_table(const int num_modules) {
    StairWaveTable* table = (StairWaveTable*)malloc(sizeof(StairWaveTable));
    if (!table) return NULL;

    table->num_points = 4 * num_modules + 2;
    table->angles = (float*)malloc(table->num_points * sizeof(float));
    table->sum_index = (int*)malloc(table->num_points * sizeof(int));
    table->sum_value = (float*)malloc(table->num_points * sizeof(float));

    
    if (!table->angles || !table->sum_index || !table->sum_value) {
        free_stair_wave_table(table);
        return NULL;
    }

    // Initialize the constant values pattern
    // First quadrant (ascending: 0 to num_modules)
    table->sum_index[0] = 0;
    table->sum_index[1] = 1;
    table->sum_index[2] = 2;
    table->sum_index[3] = 3;
    table->sum_index[4] = 4;
    table->sum_index[5] = 3;
    table->sum_index[6] = 2;
    table->sum_index[7] = 1;
    table->sum_index[8] = 0;
    table->sum_index[9] = -1;
    table->sum_index[10] = -2;
    table->sum_index[11] = -3;
    table->sum_index[12] = -4;
    table->sum_index[13] = -3;
    table->sum_index[14] = -2;
    table->sum_index[15] = -1;
    table->sum_index[16] = 0;
    table->sum_index[17] = 0;
    
    
    // for (int i = 2; i <= num_modules; i++) {
    //     table->sum_index[i] = i;
    // }

    // // Second quadrant (descending: num_modules-1 to 0)
    // for (int i = 0; i < num_modules; i++) {
    //     table->sum_index[i + num_modules + 1] = num_modules - 1 - i;
    // }

    // // Third quadrant (descending: -1 to -num_modules)
    // for (int i = 0; i <= num_modules; i++) {
    //     table->sum_index[i + 2 * num_modules + 1] = -(i + 1);
    // }

    // // Fourth quadrant (ascending: -(num_modules-1) to 0)
    // for (int i = 0; i < num_modules; i++) {
    //     table->sum_index[i + 3 * num_modules + 1] = -(num_modules - 1 - i);
    // }

    table->sum_index[table->num_points - 1] = 0;

    for (int i = 0; i < table->num_points; i++) {
        table->angles[i] = 2.0f * M_PI * i / (table->num_points - 2);
    }

    return table;
}

void update_stair_wave_table(StairWaveTable* table,
                            SwitchingAnglesTable* angles_table, 
                            single_dc_source_t* dc_sources, 
                            const int num_modules) {
    // Validate input parameters
    if (!table || !angles_table || !dc_sources || num_modules <= 0) {
        fprintf(stderr, "Invalid parameters in update_stair_wave_table\n");
        return;
    }

    // Validate table arrays
    if (!table->angles || !table->sum_index || !table->sum_value) {
        fprintf(stderr, "Invalid table arrays in update_stair_wave_table\n");
        return;
    }

    // printf("Debug: Starting update_stair_wave_table\n");

    // Calculate average voltage as the reference
    // float base_voltage = 0.0f;
    // for (int i = 0; i < num_modules; i++) {
    //     base_voltage += dc_sources[i].vdc;
    // }
    // base_voltage /= num_modules;
    
    // // Calculate voltage ratios using the average as reference
    // for (int i = 0; i < num_modules; i++) {
    //     dc_sources[i].c = dc_sources[i].vdc / base_voltage;
    // }

    // printf("xxxx dc_sources[0].vdc: %f\n", dc_sources[0].vdc );
    // printf("xxxx dc_sources[1].vdc: %f\n", dc_sources[1].vdc);
    // printf("xxxx dc_sources[2].vdc: %f\n", dc_sources[2].vdc);
    // printf("xxxx dc_sources[3].vdc: %f\n", dc_sources[3].vdc);

    SwitchingAnglesResult result;
    interpolate_switching_angles_5d_with_correction(angles_table, dc_sources, &result);
    
    // First quadrant (0 to π/2)
    table->angles[0] = 0.0f;
    float alpha1 = result.theta[0];
    float alpha2 = result.theta[1];
    float alpha3 = result.theta[2];
    float alpha4 = result.theta[3];

    table->angles[0] = 0.0f;
    table->sum_index[0] = 0;
    table->angles[1] = alpha1;
    table->angles[2] = alpha2;
    table->angles[3] = alpha3;
    table->angles[4] = alpha4;

    table->angles[5] = M_PI - alpha4;
    table->angles[6] = M_PI - alpha3;
    table->angles[7] = M_PI - alpha2;
    table->angles[8] = M_PI - alpha1;

    table->angles[9]  = M_PI + alpha1;
    table->angles[10] = M_PI + alpha2;
    table->angles[11] = M_PI + alpha3;
    table->angles[12] = M_PI + alpha4;

    table->angles[13] = 2.0f * M_PI - alpha4;
    table->angles[14] = 2.0f * M_PI - alpha3;
    table->angles[15] = 2.0f * M_PI - alpha2;
    table->angles[16] = 2.0f * M_PI - alpha1;
    table->angles[17] = 2.0f * M_PI;
    printf("xxxx alpha1: %f, alpha2: %f, alpha3: %f, alpha4: %f\n", alpha1, alpha2, alpha3, alpha4);

}

void free_stair_wave_table(StairWaveTable* table) {
    if (table) {
        free(table->angles);
        free(table->sum_index);
        free(table->sum_value);
        free(table);
    }
}
void get_switching_state(
    const StairWaveTable* table,
    float angle,
    float margin,
    bool update_table, 
    SwitchingStateResult* result
) {
    
    // Normalize angle with margin to [0, 2π]
    float angle_w_margin = fmod(angle + margin, 2.0f * M_PI);

    // Set search boundaries based on update_table flag
    int left, right;
    if (update_table || !result) {
        left = 0;
        right = table->num_points - 1;
    } else {
        // Find appropriate search window
        left = result->index;
        right = left + 2;
        // Ensure bounds are valid
        if (right >= table->num_points) {
            left = 0;
            right = table->num_points - 1;
        }

        if (table->angles[right] < angle_w_margin) {
            left = 0; 
            right = table->num_points - 1;
        }
    }

    while (left <= right) {
        // Check if angle_w_margin is within current bounds
        if (table->angles[left] <= angle_w_margin && table->angles[right] > angle_w_margin && right - left <= 1)
            break;

        int mid = (left + right) / 2;
        
        if (mid == left) break;  // Prevent infinite loop
    
        if (angle_w_margin >= table->angles[mid]) {
            left = mid;
        } else {
            right = mid - 1;
        }
    }

    result->index = left;
    result->state = table->sum_index[left];
    result->phase_2next = table->angles[right] - angle_w_margin;

    // if (right == table->num_points - 1) {
    //     result->phase_2next +=  table->angles[1];
    // }

    return;
}
