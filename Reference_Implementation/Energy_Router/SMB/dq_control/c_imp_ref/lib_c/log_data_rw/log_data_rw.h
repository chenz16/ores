#ifndef LOG_DATA_RW_H
#define LOG_DATA_RW_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Define the structure
struct LogData {
    int length;
    float *time_stamp;
    float *time_us;
    float *curr_val;
    float *curr_angle;
    float *beta;
    float *d;
    float *q;
    float *filtered_d;
    float *filtered_q;
    float *mod_index;
    float *phase_shift;
    float *computed_angle;
    float *notch_d;
    float *notch_q;
};

// Function prototypes
struct LogData* load_log_data(const char* filename);
void cleanup_data(struct LogData* data);

#endif /* LOG_DATA_RW_H */
