#ifndef GROUP_NOTCH_FILTER_H
#define GROUP_NOTCH_FILTER_H

#include "notch_filter.h"

#define MAX_FILTERS 5  // For fundamental and 3rd harmonic

typedef struct {
    float fs;
    float base_freq;
    float quality_factor;
    int harmonic_orders[MAX_FILTERS];  // Store harmonic orders [1, 3]
    NotchFilter filters[MAX_FILTERS];
} GroupNotchFilter;

void group_notch_filter_init(GroupNotchFilter* filter, float fs, float base_freq, float quality_factor);
void group_notch_filter_apply(GroupNotchFilter* filter, float* data, int data_length, float* output);

#endif // GROUP_NOTCH_FILTER_H