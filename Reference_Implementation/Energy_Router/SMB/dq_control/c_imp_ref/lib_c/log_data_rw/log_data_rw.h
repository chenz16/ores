#ifndef LOG_DATA_RW_H
#define LOG_DATA_RW_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


typedef struct {
    float* time_s;
    float* current_value;  // alpha signal
    float* current_angle;  // NEW: angle from log file
    float* beta;          // filtered result
    float* d;            // d component
    float* q;            // q component
    float* filtered_d;   // filtered d component
    float* filtered_q;   // filtered q component
    float* computed_angle;  // NEW: only addition to struct
    int length;
} LogData;


int count_lines(const char* filename);
LogData* load_log_data(const char* filename);

#endif /* LOG_DATA_RW_H */
