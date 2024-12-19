#include "log_data_rw2.h"

struct LogData* load_log_data(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        return NULL;
    }

    // Allocate memory for the structure
    struct LogData* data = (struct LogData*)malloc(sizeof(struct LogData));
    if (!data) {
        fclose(file);
        return NULL;
    }

    // First pass: count number of lines
    data->length = 0;
    char line[4096];  // Assuming max line length of 4096 characters
    while (fgets(line, sizeof(line), file)) {
        if (line[0] != '#')  // Skip comment lines
            data->length++;
    }
    data->length--;  // Subtract header line

    // Allocate memory for all arrays
    #define ALLOC_ARRAY(arr) \
        data->arr = (float*)malloc(data->length * sizeof(float)); \
        if (!data->arr) { \
            cleanup_data(data); \
            fclose(file); \
            return NULL; \
        }

    ALLOC_ARRAY(time_stamp);
    ALLOC_ARRAY(time_us);
    ALLOC_ARRAY(i_meas);
    ALLOC_ARRAY(i_alpha);
    ALLOC_ARRAY(i_beta);
    ALLOC_ARRAY(i_raw_d);
    ALLOC_ARRAY(i_raw_q);
    ALLOC_ARRAY(i_notch_d);
    ALLOC_ARRAY(i_notch_q);
    ALLOC_ARRAY(i_filtered_d);
    ALLOC_ARRAY(i_filtered_q);
    ALLOC_ARRAY(i_phase_est);
    ALLOC_ARRAY(v_grid_meas);
    ALLOC_ARRAY(v_grid_alpha);
    ALLOC_ARRAY(v_grid_beta);
    ALLOC_ARRAY(v_grid_d);
    ALLOC_ARRAY(v_grid_q);
    ALLOC_ARRAY(v_grid_phase);
    ALLOC_ARRAY(v_smb_meas);
    ALLOC_ARRAY(v_smb_alpha);
    ALLOC_ARRAY(v_smb_beta);
    ALLOC_ARRAY(v_smb_d);
    ALLOC_ARRAY(v_smb_q);
    ALLOC_ARRAY(v_smb_phase);
    ALLOC_ARRAY(v_cntl_d_ff);
    ALLOC_ARRAY(v_cntl_d_fd);
    ALLOC_ARRAY(v_cntl_q_ff);
    ALLOC_ARRAY(v_cntl_q_fd);
    ALLOC_ARRAY(v_cntl_d);
    ALLOC_ARRAY(v_cntl_q);
    ALLOC_ARRAY(v_cntl_peak);
    ALLOC_ARRAY(v_dc);
    ALLOC_ARRAY(v_cntl_mod_index);
    ALLOC_ARRAY(v_cntl_phase_shift);
    ALLOC_ARRAY(v_cntl_tgt_phase);
    ALLOC_ARRAY(v_cntl_valid);
    ALLOC_ARRAY(v_cntl_alpha);
    ALLOC_ARRAY(v_cntl_beta);

    // Reset file position to beginning
    rewind(file);

    // Skip comment lines and header
    while (fgets(line, sizeof(line), file)) {
        if (line[0] != '#')
            break;
    }

    // Read data
    for (int i = 0; i < data->length; i++) {
        if (fscanf(file, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
            &data->time_stamp[i], &data->time_us[i],
            &data->i_meas[i], &data->i_alpha[i], &data->i_beta[i],
            &data->i_raw_d[i], &data->i_raw_q[i],
            &data->i_notch_d[i], &data->i_notch_q[i],
            &data->i_filtered_d[i], &data->i_filtered_q[i], &data->i_phase_est[i],
            &data->i_ref[i], &data->i_ref_d[i], &data->i_ref_q[i],
            &data->i_ref_alpha[i], &data->i_ref_beta[i],
            &data->v_grid_meas[i], &data->v_grid_alpha[i], &data->v_grid_beta[i],
            &data->v_grid_d[i], &data->v_grid_q[i], &data->v_grid_phase[i],
            &data->v_smb_meas[i], &data->v_smb_alpha[i], &data->v_smb_beta[i],
            &data->v_smb_d[i], &data->v_smb_q[i], &data->v_smb_phase[i],
            &data->v_cntl_d_ff[i], &data->v_cntl_d_fd[i],
            &data->v_cntl_q_ff[i], &data->v_cntl_q_fd[i],
            &data->v_cntl_d[i], &data->v_cntl_q[i],
            &data->v_cntl_peak[i], &data->v_dc[i],
            &data->v_cntl_mod_index[i], &data->v_cntl_phase_shift[i],
            &data->v_cntl_tgt_phase[i], &data->v_cntl_valid[i],
            &data->v_cntl_alpha[i], &data->v_cntl_beta[i]) != 43) {
            
            fprintf(stderr, "Error reading data at line %d\n", i + 1);
            cleanup_data(data);
            fclose(file);
            return NULL;
        }
    }

    fclose(file);
    return data;
}

void cleanup_data(struct LogData* data) {
    if (data == NULL) return;

    // Free all arrays
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
    free(data->v_smb_meas);
    free(data->v_smb_alpha);
    free(data->v_smb_beta);
    free(data->v_smb_d);
    free(data->v_smb_q);
    free(data->v_smb_phase);
    free(data->v_cntl_d_ff);
    free(data->v_cntl_d_fd);
    free(data->v_cntl_q_ff);
    free(data->v_cntl_q_fd);
    free(data->v_cntl_d);
    free(data->v_cntl_q);
    free(data->v_cntl_peak);
    free(data->v_dc);
    free(data->v_cntl_mod_index);
    free(data->v_cntl_phase_shift);
    free(data->v_cntl_tgt_phase);
    free(data->v_cntl_valid);
    free(data->v_cntl_alpha);
    free(data->v_cntl_beta);
    free(data->i_ref_d);
    free(data->i_ref_q);
    free(data->i_ref_alpha);
    free(data->i_ref_beta);

    // Free the structure itself
    free(data);
}

struct LogData* init_log_data(int length) {
    if (length <= 0) {
        fprintf(stderr, "Error: Invalid length specified\n");
        return NULL;
    }

    struct LogData* data = (struct LogData*)malloc(sizeof(struct LogData));
    if (!data) {
        return NULL;
    }

    // Initialize length
    data->length = length;

    // Allocate memory for all arrays
    data->time_stamp = (float*)malloc(length * sizeof(float));
    data->time_us = (float*)malloc(length * sizeof(float));
    data->i_meas = (float*)malloc(length * sizeof(float));
    data->i_alpha = (float*)malloc(length * sizeof(float));
    data->i_beta = (float*)malloc(length * sizeof(float));
    data->i_raw_d = (float*)malloc(length * sizeof(float));
    data->i_raw_q = (float*)malloc(length * sizeof(float));
    data->i_notch_d = (float*)malloc(length * sizeof(float));
    data->i_notch_q = (float*)malloc(length * sizeof(float));
    data->i_filtered_d = (float*)malloc(length * sizeof(float));
    data->i_filtered_q = (float*)malloc(length * sizeof(float));
    data->i_phase_est = (float*)malloc(length * sizeof(float));
    data->v_grid_meas = (float*)malloc(length * sizeof(float));
    data->v_grid_alpha = (float*)malloc(length * sizeof(float));
    data->v_grid_beta = (float*)malloc(length * sizeof(float));
    data->v_grid_d = (float*)malloc(length * sizeof(float));
    data->v_grid_q = (float*)malloc(length * sizeof(float));
    data->v_grid_phase = (float*)malloc(length * sizeof(float));
    data->v_smb_meas = (float*)malloc(length * sizeof(float));
    data->v_smb_alpha = (float*)malloc(length * sizeof(float));
    data->v_smb_beta = (float*)malloc(length * sizeof(float));
    data->v_smb_d = (float*)malloc(length * sizeof(float));
    data->v_smb_q = (float*)malloc(length * sizeof(float));
    data->v_smb_phase = (float*)malloc(length * sizeof(float));
    data->v_cntl_d_ff = (float*)malloc(length * sizeof(float));
    data->v_cntl_d_fd = (float*)malloc(length * sizeof(float));
    data->v_cntl_q_ff = (float*)malloc(length * sizeof(float));
    data->v_cntl_q_fd = (float*)malloc(length * sizeof(float));
    data->v_cntl_d = (float*)malloc(length * sizeof(float));
    data->v_cntl_q = (float*)malloc(length * sizeof(float));
    data->v_cntl_peak = (float*)malloc(length * sizeof(float));
    data->v_dc = (float*)malloc(length * sizeof(float));
    data->v_cntl_mod_index = (float*)malloc(length * sizeof(float));
    data->v_cntl_phase_shift = (float*)malloc(length * sizeof(float));
    data->v_cntl_tgt_phase = (float*)malloc(length * sizeof(float));
    data->v_cntl_valid = (float*)malloc(length * sizeof(float));
    data->v_cntl_alpha = (float*)malloc(length * sizeof(float));
    data->v_cntl_beta = (float*)malloc(length * sizeof(float));
    data->i_ref_d = (float*)malloc(length * sizeof(float));
    data->i_ref_q = (float*)malloc(length * sizeof(float));
    data->i_ref_alpha = (float*)malloc(length * sizeof(float));
    data->i_ref_beta = (float*)malloc(length * sizeof(float));
    data->i_ref = (float*)malloc(length * sizeof(float));


    return data;
}