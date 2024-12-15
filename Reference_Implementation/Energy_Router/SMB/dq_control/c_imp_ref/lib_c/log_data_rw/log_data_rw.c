#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log_data_rw.h"

struct LogData* load_log_data(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return NULL;
    }

    // Count number of lines
    char buffer[1024];
    int num_lines = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        num_lines++;
    }
    rewind(file);

    // Allocate memory for the structure
    struct LogData* data = malloc(sizeof(struct LogData));
    if (!data) {
        fclose(file);
        return NULL;
    }

    // Initialize structure
    data->length = num_lines;
    data->time_stamp = malloc(num_lines * sizeof(float));
    data->time_us = malloc(num_lines * sizeof(float));
    data->curr_val = malloc(num_lines * sizeof(float));
    data->curr_angle = malloc(num_lines * sizeof(float));
    data->beta = malloc(num_lines * sizeof(float));
    data->d = malloc(num_lines * sizeof(float));
    data->q = malloc(num_lines * sizeof(float));
    data->filtered_d = malloc(num_lines * sizeof(float));
    data->filtered_q = malloc(num_lines * sizeof(float));
    data->mod_index = malloc(num_lines * sizeof(float));
    data->phase_shift = malloc(num_lines * sizeof(float));
    data->computed_angle = malloc(num_lines * sizeof(float));

    // Check all allocations
    if (!data->time_stamp || !data->time_us || !data->curr_val || !data->curr_angle ||
        !data->beta || !data->d || !data->q || !data->filtered_d || !data->filtered_q ||
        !data->mod_index || !data->phase_shift || !data->computed_angle) {
        // Cleanup on allocation failure
        cleanup_data(data);
        fclose(file);
        return NULL;
    }

    // Read data line by line
    int i = 0;
    while (i < num_lines && fgets(buffer, sizeof(buffer), file)) {
        // Skip lines that don't contain transform data
        if (strstr(buffer, "<st_ufl_transform>") == NULL) {
            continue;
        }
        
        // Parse the transform line
        if (sscanf(buffer, 
            " <st_ufl_transform>time_us:%f, curr_val:%f, curr_angle:%f, beta:%f, d:%f, q:%f, filtered_d:%f, filtered_q:%f, mod_index:%f, phase_shift:%f",
            &data->time_us[i],
            &data->curr_val[i],
            &data->curr_angle[i],
            &data->beta[i],
            &data->d[i],
            &data->q[i],
            &data->filtered_d[i],
            &data->filtered_q[i],
            &data->mod_index[i],
            &data->phase_shift[i]) != 10) {
            printf("Error parsing line %d: %s", i+1, buffer);
            continue;
        }
        
        // Extract timestamp from the ID line (previous line)
        char prev_buffer[1024];
        if (fseek(file, -strlen(buffer)-100, SEEK_CUR) >= 0 && 
            fgets(prev_buffer, sizeof(prev_buffer), file)) {
            unsigned int timestamp;
            if (sscanf(prev_buffer, "ID: %*d,T: %u", &timestamp) == 1) {
                data->time_stamp[i] = (float)timestamp;
            }
        }
        fseek(file, strlen(buffer), SEEK_CUR); // Return to original position
        
        i++;
    }

    data->length = i;  // Update to actual number of lines read
    fclose(file);
    return data;
}

void cleanup_data(struct LogData* data) {
    if (data) {
        free(data->time_stamp);
        free(data->time_us);
        free(data->curr_val);
        free(data->curr_angle);
        free(data->beta);
        free(data->d);
        free(data->q);
        free(data->filtered_d);
        free(data->filtered_q);
        free(data->mod_index);
        free(data->phase_shift);
        free(data->computed_angle);
        free(data);
    }
}