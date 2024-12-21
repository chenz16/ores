#include "beta_transform_1p.h"
#include <math.h>

void BetaTransform_1p_Init(BetaTransform_1p* beta, float target_freq, float sampling_freq) {
    // Calculate cutoff frequency for the filters
    // For 90-degree phase shift at target frequency, cutoff_freq = target_freq
    float cutoff_freq = target_freq;
    
    lpf_init(&beta->filter1, sampling_freq, cutoff_freq);
    lpf_init(&beta->filter2, sampling_freq, cutoff_freq);
    beta->beta = 0.0f;
}

float BetaTransform_1p_Update(BetaTransform_1p* beta, float alpha)
{
    float temp = lpf_process(&beta->filter1, alpha);
    temp       = lpf_process(&beta->filter2, temp);
    beta->beta = 2.0f * temp;
    return beta->beta;
}


// Update function remains the same
