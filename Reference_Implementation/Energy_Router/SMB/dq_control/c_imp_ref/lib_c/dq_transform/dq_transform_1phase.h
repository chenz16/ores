#ifndef DQ_TRANSFORM_1PHASE_H
#define DQ_TRANSFORM_1PHASE_H

#include <math.h>

/**
 * @brief Performs Park transformation from alpha-beta to d-q frame
 * 
 * @param alpha Alpha component input signal
 * @param beta Beta component input signal
 * @param angle Electrical angle in radians
 * @param d_out Pointer to store direct axis component
 * @param q_out Pointer to store quadrature axis component
 */
void dq_transform_1phase(float alpha, float beta, float angle, float *d_out, float *q_out);

void inverse_dq_transform_1phase(float d, float q, float theta, float* alpha, float* beta);
#endif /* DQ_TRANSFORM_1PHASE_H */
