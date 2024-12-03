#include "dq_transform_1phase.h"
#include <math.h>

void dq_transform_1phase(float alpha, float beta, float angle, float *d_out, float *q_out) {
    // Park transformation equations:
    *d_out = alpha * cosf(angle) + beta * sinf(angle);
    *q_out = -alpha * sinf(angle) + beta * cosf(angle);
}


void inverse_dq_transform_1phase(float d, float q, float theta,
                               float* alpha, float* beta) {
    *alpha = d * cosf(theta) - q * sinf(theta);
    *beta = d * sinf(theta) + q * cosf(theta);
}