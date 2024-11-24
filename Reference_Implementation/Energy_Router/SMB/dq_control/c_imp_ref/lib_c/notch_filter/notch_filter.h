#ifndef NOTCH_FILTER_H
#define NOTCH_FILTER_H

#define FILTER_ORDER 2

typedef struct {
    float fs;
    float base_freq;

    //ratio -> [0, 1], pole radius of the filter in the z-plane
    //  r = 0.98 narrow notch
    //  r = 0.5 wide notch
    float ratio; 
    float b_coeffs[FILTER_ORDER+1];  // b coefficients (numerator)
    float a_coeffs[FILTER_ORDER+1];  // a coefficients (denominator)
} NotchFilter;

void notch_filter_init(NotchFilter* filter, float fs, float base_freq, float ratio);
void notch_filter_create(NotchFilter* filter);
void notch_filter_apply(NotchFilter* filter, float* data, int data_length, float* output);

#endif // NOTCH_FILTER_H