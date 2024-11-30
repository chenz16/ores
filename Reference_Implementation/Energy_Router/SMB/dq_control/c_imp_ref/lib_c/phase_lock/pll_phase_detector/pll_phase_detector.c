#include "pll_phase_detector.h"

void pd_init(PhaseDetector* pd, float kd) {
    if (!pd) 
    { 
        pd->error_code = PD_ERROR_NULL_POINTER; 
        return ;
    }
    pd->kd = kd;
    pd->phase_error = 0.0f;
    pd->error_code = PD_ERROR_NONE;
}

int8_t pd_update(PhaseDetector* pd, float grid_voltage, float vco_cos, float *output) {
    if (!pd) {
        pd->error_code = PD_ERROR_NULL_POINTER; 
        pd->phase_error = 0.0f;
    }
    else{ 
        // Multiply grid voltage with VCO cosine
        pd->phase_error = pd->kd * grid_voltage * vco_cos;
        pd->error_code = PD_ERROR_NONE;
    }
    *output = pd->phase_error;
    return pd->error_code;       
}

float pd_get_error(const PhaseDetector* pd) {
    return pd->phase_error;
}