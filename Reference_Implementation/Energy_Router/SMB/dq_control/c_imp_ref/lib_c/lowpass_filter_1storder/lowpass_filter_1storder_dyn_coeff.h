#ifndef LOWPASS_FILTER_1STORDER_DYN_COEFF_H
#define LOWPASS_FILTER_1STORDER_DYN_COEFF_H

#include <math.h>
#include "lowpass_filter_1storder.h"

typedef struct {
    LowPassFilter1st signal_filter;    // Filter for the input signal
    LowPassFilter1st coeff_filter;     // Filter for the cutoff frequency
    float fs;                          // Sampling freq
    float fc_start;                    // Starting cutoff frequency
    float fc_end;                      // Target cutoff frequency
} LowPassFilter1st_dyn_coeff;

static inline void lpf_dyn_coeff_init(LowPassFilter1st_dyn_coeff* filter,
                                   float fs,
                                   float fc_start,
                                   float fc_end)
{
    filter->fs = fs;
    filter->fc_start = fc_start;
    filter->fc_end = fc_end;
    
    // Initialize the signal filter with starting frequency
    lpf_init(&filter->signal_filter, fs, fc_start);
    
    // Initialize the coefficient filter with a slower response
    // Using a lower frequency for smooth coefficient transition
    lpf_init(&filter->coeff_filter, fs, fc_start);  // Slower dynamics for coefficient changes
    lpf_set_value(&filter->coeff_filter, fc_start, fc_start);
}

static inline float lpf_dyn_coeff_process(LowPassFilter1st_dyn_coeff* filter, float input) {
    // Update the cutoff frequency dynamically
    float current_fc = lpf_process(&filter->coeff_filter, filter->fc_end);
    lpf_create_coeff(&filter->signal_filter, current_fc, filter->fs);
    // Process the input signal with updated coefficient
    return lpf_process(&filter->signal_filter, input);
}

#endif // LOWPASS_FILTER_1STORDER_H