#ifndef PLL_PHASE_DETECTOR_H
#define PLL_PHASE_DETECTOR_H
#include <stdio.h>
#include <stdint.h>  // Add this header


// Error codes
#define PD_ERROR_NONE 0
#define PD_ERROR_NULL_POINTER -1

typedef struct {
    float kd;           // Phase detector gain
    float phase_error;  // Phase error output
    int8_t error_code;
} PhaseDetector;

// Function prototypes
void pd_init(PhaseDetector* pd, float kd);
int8_t pd_update(PhaseDetector* pd, float grid_voltage, float vco_cos, float * output);
float pd_get_error(const PhaseDetector* pd);

#endif // PLL_PHASE_DETECTOR_