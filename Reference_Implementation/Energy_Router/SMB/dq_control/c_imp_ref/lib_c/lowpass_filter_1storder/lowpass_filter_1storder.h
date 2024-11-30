
#ifndef LOWPASS_FILTER_1STORDER_H
#define LOWPASS_FILTER_1STORDER_H

#include <math.h>

typedef struct {
    float b0;          // Filter coefficient b0
    float b1;          // Filter coefficient b1
    float a1;          // Filter coefficient a1
    float prev_input;  // Previous input sample
    float prev_output; // Previous output sample
} LowPassFilter1st;

// Initialize the filter with sampling frequency (fs) and cutoff frequency (fc)
static inline void lpf_init(LowPassFilter1st* filter, float fs, float fc) {
    const float pi = 3.14159265359f;

    if(fc > fs/2.0f) {
        printf("LPF Error - Cutoff frequency is greater than half the sampling frequency\n");
        exit(1);
    }
    
    // Pre-warp the cutoff frequency
    float wc = 2.0f * fs * tanf(pi * fc / fs);
    
    // Bilinear transform coefficients
    float k = 2.0f * fs;
    filter->b0 = wc / (k + wc);
    filter->b1 = filter->b0;
    filter->a1 = (wc - k) / (k + wc);
    
    // Initialize state variables
    filter->prev_input = 0.0f;
    filter->prev_output = 0.0f;
}

// Process a single sample through the filter
static inline float lpf_process(LowPassFilter1st* filter, float input) {
    float output = filter->b0 * input + 
                  filter->b1 * filter->prev_input - 
                  filter->a1 * filter->prev_output;
    
    // Update state variables
    filter->prev_input = input;
    filter->prev_output = output;

    return output;
}

#endif // LOWPASS_FILTER_1STORDER_H