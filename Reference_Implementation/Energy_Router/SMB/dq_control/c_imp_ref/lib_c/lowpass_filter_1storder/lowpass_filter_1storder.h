
#ifndef LOWPASS_FILTER_1STORDER_H
#define LOWPASS_FILTER_1STORDER_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    float b0;          // Filter coefficient b0
    float b1;          // Filter coefficient b1
    float a1;          // Filter coefficient a1
    float prev_input;  // Previous input sample
    float prev_output; // Previous output sample
    bool flag_init;
} LowPassFilter1st;


static inline void lpf_create_coeff(LowPassFilter1st* filter, float fc, float fs) {
    const float pi = 3.14159265359f;

    if(fc > fs/2.0f) {
        printf("LPF Error - Cutoff frequency is greater than half the sampling frequency\n");
        #ifndef COMPILE_APP_Program
        exit(1);
        #endif
    }
    // Pre-warp the cutoff frequency
    float wc = 2.0f * fs * tanf(pi * fc / fs);
    // Bilinear transform coefficients
    float k = 2.0f * fs;
    filter->b0 = wc / (k + wc);
    filter->b1 = filter->b0;
    filter->a1 = (wc - k) / (k + wc);
}

static inline void lpf_set_value(LowPassFilter1st* filter, float prev_output, float prev_input) {
    filter->prev_input = prev_input;
    filter->prev_output = prev_output;
}

static inline void lpf_reset_init_flag(LowPassFilter1st* filter) {
    filter->flag_init = false;
}

// Initialize the filter with sampling frequency (fs) and cutoff frequency (fc)
static inline void lpf_init(LowPassFilter1st* filter, float fs, float fc) {
    lpf_create_coeff(filter, fc, fs);
    lpf_set_value(filter, 0, 0);
    filter->flag_init = false;
}


// Process a single sample through the filter
static inline float lpf_process(LowPassFilter1st* filter, float input) {

    if(!filter->flag_init) {
        lpf_set_value(filter, input, input);
        filter->flag_init = true;
        return input;
    }

    float output = filter->b0 * input + 
                  filter->b1 * filter->prev_input - 
                  filter->a1 * filter->prev_output;
    
    // Update state variables
    filter->prev_input = input;
    filter->prev_output = output;

    return output;
}

#endif // LOWPASS_FILTER_1STORDER_H

