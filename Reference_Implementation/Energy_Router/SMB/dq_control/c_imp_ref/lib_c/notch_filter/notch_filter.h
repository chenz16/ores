#ifndef NOTCH_FILTER_H
#define NOTCH_FILTER_H

#define FILTER_ORDER 2

#include <stdbool.h>

typedef struct {
    float fs;
    float base_freq;

    //ratio -> [0, 1], pole radius of the filter in the z-plane
    //  r = 0.98 narrow notch
    //  r = 0.5 wide notch
    float ratio; 
    float b_coeffs[FILTER_ORDER+1];  // b coefficients (numerator)
    float a_coeffs[FILTER_ORDER+1];  // a coefficients (denominator)
    float x1;  // Previous input
    float x2;  // Second previous input
    float y1;  // Previous output
    float y2;  // Second previous output
    bool flag_init;
} NotchFilter;

void notch_filter_init(NotchFilter* filter, float fs, float base_freq, float ratio);
void notch_filter_create(NotchFilter* filter);
float notch_filter_apply(NotchFilter* filter, float input);
static inline void notch_filter_reset_init_flag(NotchFilter* filter) {
    filter->flag_init = false;
}

#endif // NOTCH_FILTER_H
