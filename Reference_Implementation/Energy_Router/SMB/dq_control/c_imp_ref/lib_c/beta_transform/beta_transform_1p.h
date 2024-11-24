#ifndef BETA_TRANSFORM_H
#define BETA_TRANSFORM_H

#include "../lowpass_filter_1storder/lowpass_filter_1storder.h"

typedef struct {
    LowPassFilter1st filter1;
    LowPassFilter1st filter2;
    float beta;
} BetaTransform_1p;

void BetaTransform_1p_Init(BetaTransform_1p* beta, float target_freq, float sampling_freq);
float BetaTransform_1p_Update(BetaTransform_1p* beta, float alpha);

#endif /* BETA_TRANSFORM_H */
