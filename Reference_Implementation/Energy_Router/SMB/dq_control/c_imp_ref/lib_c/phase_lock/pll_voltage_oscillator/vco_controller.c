#include <stdio.h>
#include "vco_controller.h"
#include <math.h>

extern volatile float g_control_signal;
extern volatile int g_signal_valid;

vco_error_t vco_controller_init(vco_controller_t* vco, 
                               float ts, 
                               float nominal_freq,
                               float k0) {
    if (!vco) return VCO_ERROR_NULL_POINTER;
    if (ts <= 0.0f || nominal_freq <= 0.0f || k0 <= 0.0f) 
        return VCO_ERROR_INVALID_PARAMETER;
    
    vco->ts = ts;
    vco->nominal_freq = nominal_freq;
    vco->k0 = k0;
    
    // Initialize states
    vco->phase = 0.0f;
    vco->phase_nominal = 0.0f;
    vco->phase_correction = 0.0f;
    vco->frequency = nominal_freq;
    vco->freq_correction = 0.0f;
    
    return VCO_ERROR_NONE;
}

vco_error_t vco_controller_update(vco_controller_t* vco, float control_signal) {
    if (!vco) return VCO_ERROR_NULL_POINTER;
    
    // Update frequency
    vco->freq_correction = vco->k0 * control_signal;
    vco->frequency = vco->nominal_freq + vco->freq_correction;
    
    // Integrate frequency to get phase
    vco->phase += 2.0f * M_PI * vco->frequency * vco->ts;
    
    return VCO_ERROR_NONE;
}

vco_error_t vco_controller_reset(vco_controller_t* vco) {
    if (!vco) return VCO_ERROR_NULL_POINTER;
    
    vco->phase = 0.0f;
    vco->phase_nominal = 0.0f;
    vco->phase_correction = 0.0f;
    vco->frequency = vco->nominal_freq;
    vco->freq_correction = 0.0f;
    
    return VCO_ERROR_NONE;
}

vco_error_t vco_controller_set_nominal_freq(vco_controller_t* vco, float nominal_freq) {
    if (!vco) return VCO_ERROR_NULL_POINTER;
    
    vco->nominal_freq = nominal_freq;
    vco->frequency = nominal_freq + vco->freq_correction;
    
    return VCO_ERROR_NONE;
}

float vco_controller_get_phase(const vco_controller_t* vco) {
    return vco->phase;
}

float vco_controller_get_frequency(const vco_controller_t* vco) {
    return vco->frequency;
}