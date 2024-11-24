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
    
    // printf("\nNotch Filter (%.1f Hz):\n", filter->base_freq);
    // printf("b: [%.6f, %.6f, %.6f]\n", 
    //        filter->b_coeffs[0], filter->b_coeffs[1], filter->b_coeffs[2]);
    // printf("a: [%.6f, %.6f, %.6f]\n", 
    //        filter->a_coeffs[0], filter->a_coeffs[1], filter->a_coeffs[2]);
}

void notch_filter_apply(NotchFilter* filter, float* data, int data_length, float* output) {
    float x1 = 0.0f, x2 = 0.0f;
    float y1 = 0.0f, y2 = 0.0f;
    
    for (int n = 0; n < data_length; n++) {
        float x0 = data[n];
        
        // Direct form II difference equation
        float y0 = filter->b_coeffs[0] * x0 + filter->b_coeffs[1] * x1 + filter->b_coeffs[2] * x2
                  - filter->a_coeffs[1] * y1 - filter->a_coeffs[2] * y2;
        
        // Update delays
        x2 = x1;
        x1 = x0;
        y2 = y1;
        y1 = y0;
        
        output[n] = y0;
    }
}
