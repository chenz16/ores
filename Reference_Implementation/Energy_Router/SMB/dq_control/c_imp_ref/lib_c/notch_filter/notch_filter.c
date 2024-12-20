#include "notch_filter.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

void notch_filter_init(NotchFilter* filter, float fs, float base_freq, float ratio) {
    filter->fs = fs;
    filter->base_freq = base_freq;
    filter->ratio = ratio;
    
    // Initialize coefficient arrays to zero
    memset(filter->b_coeffs, 0, sizeof(float) * FILTER_ORDER + 1);
    memset(filter->a_coeffs, 0, sizeof(float) * FILTER_ORDER + 1);
    
    // Initialize state variables
    filter->x1 = 0.0f;
    filter->x2 = 0.0f;
    filter->y1 = 0.0f;
    filter->y2 = 0.0f;
    filter->flag_init = false;
    notch_filter_create(filter);
}

void notch_filter_create(NotchFilter* filter) {
    float w0 = 2.0f * M_PI * filter->base_freq / filter->fs;
    float cos_w0 = cosf(w0);
    float r = filter->ratio;  // Pole radius, controls notch width
    
    // Simple notch filter coefficients
    filter->b_coeffs[0] = 1.0f;                // b0
    filter->b_coeffs[1] = -2.0f * cos_w0;      // b1
    filter->b_coeffs[2] = 1.0f;                // b2
    
    filter->a_coeffs[0] = 1.0f;                // a0
    filter->a_coeffs[1] = -2.0f * r * cos_w0;  // a1
    filter->a_coeffs[2] = r * r;               // a2
    
}

float notch_filter_apply(NotchFilter* filter, float input) {

    if(!filter->flag_init) {
        filter->x1 = input;
        filter->x2 = input;
        filter->y1 = input;
        filter->y2 = input;
        filter->flag_init = true;
        return input;
    }

    float y0 = filter->b_coeffs[0] * input + filter->b_coeffs[1] * filter->x1 + filter->b_coeffs[2] * filter->x2
              - filter->a_coeffs[1] * filter->y1 - filter->a_coeffs[2] * filter->y2;
    
    // Update delays
    filter->x2 = filter->x1;
    filter->x1 = input;
    filter->y2 = filter->y1;
    filter->y1 = y0;
    return y0;
}