#ifndef PLL_H
#define PLL_H

// Include all required type definitions
#include "pll_phase_detector/pll_phase_detector.h"  // Should define phase_detector_t
#include "pll_controller_pi/pll_controller_pi.h"    // Should define pi_controller_t
#include "pll_voltage_oscillator/vco_controller.h"  // Should define vco_controller_t
#include "../notch_filter/notch_filter.h"          // Should define notch_filter_t
#include "../lowpass_filter_1storder/lowpass_filter_1storder.h"  // Should define lowpass_filter_1storder_t
#include "../lowpass_filter_1storder/lowpass_filter_1storder_dyn_coeff.h"

// Error codes
#define PLL_SUCCESS 0
#define PLL_ERROR_NULL_POINTER -1
#define PLL_ERROR_INVALID_PARAMETER -2

// Notch filter configuration
#define MAX_NOTCH_FILTERS 5      // Maximum number of notch filters
#define NOTCH_2ND_ORDER 2        // 2nd order harmonic
#define NOTCH_3RD_ORDER 3        // 3rd order harmonic
#define NOTCH_4TH_ORDER 4        // 4th order harmonic
#define NOTCH_5TH_ORDER 5        // 5th order harmonic
#define NOTCH_6TH_ORDER 6        // 6th order harmonic

// Notch filter enable flags (can be OR'ed together)
#define NOTCH_NONE      0x00
#define NOTCH_2ND      (1 << 0)
#define NOTCH_3RD      (1 << 1)
#define NOTCH_4TH      (1 << 2)
#define NOTCH_5TH      (1 << 3)
#define NOTCH_6TH      (1 << 4)

typedef struct {
    float sampling_freq;
    float base_freq;
    float ts;
    
    PhaseDetector phase_detector;
    NotchFilter error_notch[MAX_NOTCH_FILTERS];
    // LowPassFilter1st error_lpf;
    LowPassFilter1st_dyn_coeff error_lpf;

    pi_controller_t pi_controller;
    vco_controller_t vco;
    
    int active_notches;
    int num_active_notches;
    float output_phase_detector;
    float output_notch_filter;
    float output_lpf;
    float output_pi;
    float output_vco_correction_freq;
    float output_vco_applied_freq;
    float output_vco_phase;
    // float filtered_error;
    // float current_control_signal;
} PLL;

// Function prototypes
int pll_init(PLL* pll, 
            float sampling_freq,
            float nominal_freq,
            const float* notch_ratios,
            int notch_config,
            float lpf_cutoff_freq,
            float kd,
            float kp,
            float ki,
            float freq_max,
            float freq_min,
            float k0, 
            float initial_phase);

void pll_cleanup(PLL* pll);         // New cleanup function
int pll_update(PLL* pll, float grid_voltage);
void pll_apply_notch_filter(PLL* pll, float grid_voltage);

#endif // PLL_H
