#ifndef LOG_DATA_RW2_H
#define LOG_DATA_RW2_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Define the structure
struct LogData {
    int length;
    float *time_stamp;
    float *time_us;

    float *i_meas;
    float *i_alpha;
    float *i_beta;
    float *i_raw_d;
    float *i_raw_q;
    float *i_notch_d;
    float *i_notch_q;
    float *i_filtered_d;
    float *i_filtered_q;
    float *i_phase_est;

    float *v_grid_meas;
    float *v_grid_alpha;
    float *v_grid_beta;
    float *v_grid_d;
    float *v_grid_q;
    float *v_grid_phase;

    float *v_cntl_d_ff;
    float *v_cntl_d_fd;
    float *v_cntl_q_ff;
    float *v_cntl_q_fd;

    float *v_cntl_d;
    float *v_cntl_q;
    float *v_cntl_peak;
    float *v_cntl_phase_shift;
    float *vdc;

};

// Function prototypes
struct LogData* load_log_data(const char* filename);
void cleanup_data(struct LogData* data);

#endif /* LOG_DATA_RW_H */
