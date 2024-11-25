#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../beta_transform/beta_transform_1p.h"
#include "../../dq_transform/dq_transform_1phase.h"
#include "../../lowpass_filter_1storder/lowpass_filter_1storder.h"

// Structure to hold the log data
typedef struct {
    float* time_s;
    float* current_value;  // alpha signal
    float* current_angle;  // NEW: angle from log file
    float* beta;          // filtered result
    float* d;            // d component
    float* q;            // q component
    float* filtered_d;   // filtered d component
    float* filtered_q;   // filtered q component
    int length;
} LogData;

// Helper function to count lines in file
int count_lines(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) return -1;
    
    int lines = 0;
    char buffer[1024];
    
    // Skip header
    fgets(buffer, sizeof(buffer), file);
    
    // Count data lines
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        lines++;
    }
    
    fclose(file);
    return lines;
}

// Function to load data from CSV file
LogData* load_log_data(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return NULL;
    }
    
    int num_lines = count_lines(filename);
    if (num_lines <= 0) {
        printf("Error: Empty or invalid file\n");
        fclose(file);
        return NULL;
    }
    
    LogData* data = malloc(sizeof(LogData));
    data->length = num_lines;
    data->time_s = malloc(num_lines * sizeof(float));
    data->current_value = malloc(num_lines * sizeof(float));
    data->current_angle = malloc(num_lines * sizeof(float));
    data->beta = NULL;
    
    // Skip header and store it for verification
    char buffer[1024];
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        printf("Error reading header\n");
        free(data->time_s);
        free(data->current_value);
        free(data->current_angle);
        free(data);
        fclose(file);
        return NULL;
    }
    printf("Header: %s", buffer);  // Print header for verification
    
    // Read data line by line
    int i = 0;
    while (i < num_lines && fgets(buffer, sizeof(buffer), file)) {
        // Print first few lines for debugging
        if (i < 5) {
            printf("Line %d: %s", i+1, buffer);
        }
        
        if (sscanf(buffer, "%f,%f,%f", 
                   &data->time_s[i], 
                   &data->current_value[i],
                   &data->current_angle[i]) != 3) {
            printf("Error parsing line %d: %s", i+1, buffer);
            continue;
        }
        i++;
    }
    
    data->length = i;  // Update to actual number of lines read
    printf("Successfully read %d lines\n", i);
    
    fclose(file);
    return data;
}

// Function to create 90-degree phase shift using beta transform
void process_signals(LogData* data) {
    // Initialize all components
    data->beta = malloc(data->length * sizeof(float));
    data->d = malloc(data->length * sizeof(float));
    data->q = malloc(data->length * sizeof(float));
    data->filtered_d = malloc(data->length * sizeof(float));
    data->filtered_q = malloc(data->length * sizeof(float));
    
    // Initialize transforms and filters
    BetaTransform_1p beta_transform;
    LowPassFilter1st filter_d, filter_q;
    
    float sample_freq = 1000.0f;  // 1kHz
    float target_freq = 50.0f;    // 50Hz
    float fc = 10.0f;            // 10Hz cutoff frequency
    float omega = 2.0f * M_PI * target_freq;
    
    // Initialize components
    BetaTransform_1p_Init(&beta_transform, target_freq, sample_freq);
    lpf_init(&filter_d, sample_freq, fc);
    lpf_init(&filter_q, sample_freq, fc);
    
    // Process each sample sequentially
    for (int i = 0; i < data->length; i++) {
        // Step 1: Generate beta from alpha
        float beta = BetaTransform_1p_Update(&beta_transform, data->current_value[i]);
        data->beta[i] = beta;
        
        // Step 2: Calculate theta for current time
        // float theta = omega * data->time_s[i];
        float theta = data->current_angle[i];
        
        // Step 3: Park transform
        float d, q;
        dq_transform_1phase(data->current_value[i], beta, theta, &d, &q);
        data->d[i] = d;
        data->q[i] = q;
        
        // Step 4: Apply low-pass filtering
        data->filtered_d[i] = lpf_process(&filter_d, d);
        data->filtered_q[i] = lpf_process(&filter_q, q);
    }
}

// Function to save the processed data
void save_data(const LogData* data, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening output file\n");
        return;
    }
    
    // Updated header to include filtered components
    fprintf(file, "time_s,current_value,current_angle,beta,d,q,filtered_d,filtered_q\n");
    
    for (int i = 0; i < data->length; i++) {
        fprintf(file, "%f,%f,%f,%f,%f,%f,%f,%f\n", 
                data->time_s[i], 
                data->current_value[i],
                data->current_angle[i],
                data->beta[i],
                data->d[i],
                data->q[i],
                data->filtered_d[i],
                data->filtered_q[i]);
    }
    
    fclose(file);
}

void cleanup_data(LogData* data) {
    free(data->time_s);
    free(data->current_value);
    free(data->current_angle);
    free(data->beta);
    free(data->d);
    free(data->q);
    free(data->filtered_d);
    free(data->filtered_q);
    free(data);
}

int main() {
    const char* input_file = "log_data.csv";
    const char* output_file = "log_data_with_beta.csv";
    
    printf("Loading data from %s...\n", input_file);
    LogData* data = load_log_data(input_file);
    if (data == NULL) {
        printf("Error loading data\n");
        return 1;
    }
    printf("Loaded %d data points\n", data->length);
    
    printf("Processing signals...\n");
    process_signals(data);
    
    printf("Saving results to %s...\n", output_file);
    save_data(data, output_file);
    
    cleanup_data(data);
    
    return 0;
}