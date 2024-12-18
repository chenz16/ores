#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log_data_rw2.h"

struct LogData* load_log_data(const char* filename) {
    struct LogData* data = (struct LogData*)malloc(sizeof(struct LogData));
    if (!data) {
        return NULL;
    }

    // Initialize all pointers to NULL
    data->length = 0;
    data->time_stamp = NULL;
    data->time_us = NULL;
    data->i_meas = NULL;
    data->i_alpha = NULL;
    data->i_beta = NULL;
    data->i_raw_d = NULL;
    data->i_raw_q = NULL;
    data->i_notch_d = NULL;
    data->i_notch_q = NULL;
    data->i_filtered_d = NULL;
    data->i_filtered_q = NULL;
    data->i_phase_est = NULL;
    data->v_grid_meas = NULL;
    data->v_grid_alpha = NULL;
    data->v_grid_beta = NULL;
    data->v_grid_d = NULL;
    data->v_grid_q = NULL;
    data->v_grid_phase = NULL;
    data->v_cntl_d_ff = NULL;
    data->v_cntl_d_fd = NULL;
    data->v_cntl_q_ff = NULL;
    data->v_cntl_q_fd = NULL;
    data->v_cntl_d = NULL;
    data->v_cntl_q = NULL;
    data->v_cntl_peak = NULL;
    data->v_cntl_phase_shift = NULL;
    data->vdc = NULL;

    return data;
}

void cleanup_data(struct LogData* data) {
    if (data) {
        // Free all allocated arrays
        free(data->time_stamp);
        free(data->time_us);
        free(data->i_meas);
        free(data->i_alpha);
        free(data->i_beta);
        free(data->i_raw_d);
        free(data->i_raw_q);
        free(data->i_notch_d);
        free(data->i_notch_q);
        free(data->i_filtered_d);
        free(data->i_filtered_q);
        free(data->i_phase_est);
        free(data->v_grid_meas);
        free(data->v_grid_alpha);
        free(data->v_grid_beta);
        free(data->v_grid_d);
        free(data->v_grid_q);
        free(data->v_grid_phase);
        free(data->v_cntl_d_ff);
        free(data->v_cntl_d_fd);
        free(data->v_cntl_q_ff);
        free(data->v_cntl_q_fd);
        free(data->v_cntl_d);
        free(data->v_cntl_q);
        free(data->v_cntl_peak);
        free(data->v_cntl_phase_shift);
        free(data->vdc);

        // Free the structure itself
        free(data);
    }
}