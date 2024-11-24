#include "group_notch_filter.h"
#include "notch_filter.h"
#include <string.h>

void group_notch_filter_init(GroupNotchFilter* filter, float fs, float base_freq, float quality_factor) {
    filter->fs = fs;
    filter->base_freq = base_freq;
    filter->quality_factor = quality_factor;
    
    // Initialize individual notch filters for each harmonic
    notch_filter_init(&filter->filters[0], fs, base_freq, quality_factor);        // 50Hz
    notch_filter_init(&filter->filters[1], fs, base_freq * 3, quality_factor);    // 150Hz (3rd harmonic)
}

void group_notch_filter_apply(GroupNotchFilter* filter, float* data, int data_length, float* output) {
    // Create temporary buffer
    float* temp = (float*)malloc(data_length * sizeof(float));
    memcpy(temp, data, data_length * sizeof(float));
    
    // Apply each notch filter in sequence
    for (int i = 0; i < 2; i++) {  // Apply both 50Hz and 150Hz filters
        notch_filter_apply(&filter->filters[i], temp, data_length, output);
        memcpy(temp, output, data_length * sizeof(float));
    }
    
    free(temp);
}