#include "load_switching_angles_table_5d.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

SwitchingAnglesTable* load_switching_angles_table_5d(void) {
    SwitchingAnglesTable* table = (SwitchingAnglesTable*)malloc(sizeof(SwitchingAnglesTable));
    if (!table) {
        fprintf(stderr, "Error: Failed to allocate table memory\n");
        return NULL;
    }

    // Get directory path from __FILE__
    char dir_path[512];
    strncpy(dir_path, __FILE__, sizeof(dir_path) - 1);
    dir_path[sizeof(dir_path) - 1] = '\0';  // Ensure null termination
    char* last_slash = strrchr(dir_path, '/');
    if (last_slash) {
        *last_slash = '\0';  // Remove filename, keep directory path
    }

    // Construct the full path
    char filename[1024];  // Increased buffer size
    int ret = snprintf(filename, sizeof(filename), "%s/switching_angles_table.bin", dir_path);
    if (ret < 0 || (size_t)ret >= sizeof(filename)) {
        fprintf(stderr, "Error: Path too long for buffer\n");
        free(table);
        return NULL;
    }
    
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Error: Could not open file '%s'\n", filename);
        free(table);
        return NULL;
    }

    // Read dimensions
    int32_t dimensions[2];  // [N_POINTS_PER_CI, N_MOD_INDEX]
    if (fread(dimensions, sizeof(int32_t), 2, fp) != 2) {
        fprintf(stderr, "Error: Failed to read dimensions\n");
        fclose(fp);
        free(table);
        return NULL;
    }

    // Verify dimensions match expected values
    if (dimensions[0] != N_POINTS_PER_CI || dimensions[1] != N_MOD_INDEX) {
        fprintf(stderr, "Error: File dimensions (%d, %d) don't match expected (%d, %d)\n",
                dimensions[0], dimensions[1], N_POINTS_PER_CI, N_MOD_INDEX);
        fclose(fp);
        free(table);
        return NULL;
    }

    // printf("Debug: Table dimensions: ci_points=%d, m_points=%d\n", dimensions[0], dimensions[1]);
    // printf("Debug: First few ci values: %.3f, %.3f, %.3f\n", 
    //        table->c1_values[0], table->c1_values[1], table->c1_values[2]);
    // printf("Debug: First few m values: %.3f, %.3f, %.3f\n", 
    //        table->m_values[0], table->m_values[1], table->m_values[2]);

    // Read ci_options and m_options
    if (fread(table->c1_values, sizeof(float), N_POINTS_PER_CI, fp) != N_POINTS_PER_CI ||
        fread(table->m_values, sizeof(float), N_MOD_INDEX, fp) != N_MOD_INDEX) {
        fprintf(stderr, "Error: Failed to read parameter values\n");
        fclose(fp);
        free(table);
        return NULL;
    }
    
    // Copy ci values to other arrays
    memcpy(table->c2_values, table->c1_values, N_POINTS_PER_CI * sizeof(float));
    memcpy(table->c3_values, table->c1_values, N_POINTS_PER_CI * sizeof(float));
    memcpy(table->c4_values, table->c1_values, N_POINTS_PER_CI * sizeof(float));

    // Calculate total size for one 5D array
    const size_t total_size = N_POINTS_PER_CI * N_POINTS_PER_CI * N_POINTS_PER_CI * 
                             N_POINTS_PER_CI * N_MOD_INDEX;

    // Temporary buffer to hold one 5D table
    float* temp_buffer = (float*)malloc(total_size * sizeof(float));
    if (!temp_buffer) {
        fprintf(stderr, "Error: Failed to allocate temporary buffer\n");
        fclose(fp);
        free(table);
        return NULL;
    }

    // Read and reorganize the four theta tables into the 6D angles array
    for (int dc = 0; dc < NUM_DC_SOURCES; dc++) {
        // Read one 5D theta table
        if (fread(temp_buffer, sizeof(float), total_size, fp) != (size_t)total_size) {
            fprintf(stderr, "Error: Failed to read theta table %d\n", dc + 1);
            free(temp_buffer);
            fclose(fp);
            free(table);
            return NULL;
        }

        // Copy to the appropriate positions in the 6D array
        for (int i1 = 0; i1 < N_POINTS_PER_CI; i1++) {
            for (int i2 = 0; i2 < N_POINTS_PER_CI; i2++) {
                for (int i3 = 0; i3 < N_POINTS_PER_CI; i3++) {
                    for (int i4 = 0; i4 < N_POINTS_PER_CI; i4++) {
                        for (int im = 0; im < N_MOD_INDEX; im++) {
                            size_t src_idx = ((((i1 * N_POINTS_PER_CI + i2) * N_POINTS_PER_CI + i3) 
                                           * N_POINTS_PER_CI + i4) * N_MOD_INDEX + im);
                            table->angles[i1][i2][i3][i4][im][dc] = temp_buffer[src_idx];
                        }
                    }
                }
            }
        }
    }

    free(temp_buffer);

    // Read THD and V1 error tables
    if (fread(&table->thd[0][0][0][0][0], sizeof(float), total_size, fp) != (size_t)total_size ||
        fread(&table->v1_error[0][0][0][0][0], sizeof(float), total_size, fp) != (size_t)total_size) {
        fclose(fp);
        free(table);
        return NULL;
    }

    fclose(fp);
    return table;
}

void free_switching_angles_table_5d(SwitchingAnglesTable* table) {
    if (table) {
        free(table);
    }
}
