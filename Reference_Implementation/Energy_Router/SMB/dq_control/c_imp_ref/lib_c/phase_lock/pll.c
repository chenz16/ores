#include "pll_phase_detector/pll_phase_detector.h"
#include "pll.h"
#include <math.h>
#include "../lowpass_filter_1storder/lowpass_filter_1storder.h"
#include "../notch_filter/notch_filter.h"
#include "./pll_voltage_oscillator/vco_controller.h"
#include "./pll_controller_pi/pll_controller_pi.h"
#include "./pll_phase_detector/pll_phase_detector.h"


static float calculate_harmonic_freq(float base_freq, int harmonic_order) {
    return base_freq * (float)harmonic_order;
}

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
            float initial_phase) {
    
    if (!pll) return PLL_ERROR_NULL_POINTER;
    // Only check notch_ratios if we have active notches
    if (notch_config != NOTCH_NONE && !notch_ratios) return PLL_ERROR_NULL_POINTER;
    
    // Store frequencies
    pll->sampling_freq = sampling_freq;
    pll->base_freq = nominal_freq;
    pll->ts = 1.0f / sampling_freq;
    
    // Initialize phase detector
    pd_init(&pll->phase_detector, kd);
    
    // Initialize notch filters based on configuration
    pll->active_notches = notch_config;
    pll->num_active_notches = 0;
    
    // Only create notch filters if we have an active configuration
    if (notch_config != NOTCH_NONE) {
        int filter_idx = 0;
        for (int order = NOTCH_2ND_ORDER; order <= NOTCH_6TH_ORDER; order++) {
            if (notch_config & (1 << (order - NOTCH_2ND_ORDER))) {
                float harmonic_freq = calculate_harmonic_freq(pll->base_freq, order);
                float ratio = notch_ratios[order - NOTCH_2ND_ORDER];
                notch_filter_init(&pll->error_notch[filter_idx],
                                pll->sampling_freq,
                                harmonic_freq,
                                ratio);
                pll->num_active_notches++;
                filter_idx++;
            }
        }
    }
    
    // Initialize lowpass filter
    // lpf_init(&pll->error_lpf, sampling_freq, lpf_cutoff_freq);
    lpf_dyn_coeff_init(&pll->error_lpf, sampling_freq, 1.0f*lpf_cutoff_freq, lpf_cutoff_freq);
    // Initialize PI controller
    pi_controller_init(&pll->pi_controller, kp, ki, pll->ts, 
                      freq_max - nominal_freq, 
                      freq_min - nominal_freq);
    
    // Initialize VCO with k0
    vco_controller_init(&pll->vco, pll->ts, nominal_freq, k0, initial_phase);

    pll->output_phase_detector = pll->phase_detector.phase_error;
    pll->output_notch_filter = 0.0;
    pll->output_lpf = 0.0f;
    pll->output_pi = 0.0f;
    pll->output_vco_correction_freq = 0.0f;
    pll->output_vco_applied_freq = nominal_freq;
    pll->output_vco_phase = pll->vco.phase;
    // printf("VCO phase: %f\n", pll->output_vco_phase);
    // exit(0);
    
    return PLL_SUCCESS;
}

void pll_cleanup(PLL* pll) {
    (void)pll;  // Silence unused parameter warning
}

int pll_update(PLL* pll, float grid_voltage) {
    if (pll == NULL) return PLL_ERROR_NULL_POINTER;

    // Print values before update
    // printf("Before PLL Update:\n");
    // printf("  Phase Detector Output: %.4f\n", pll->output_phase_detector);
    // printf("  Notch Filter Output:   %.4f\n", pll->output_notch_filter);
    // printf("  LPF Output:            %.4f\n", pll->output_lpf);
    // printf("  PI Output:             %.4f\n", pll->output_pi);
    // printf("  VCO Correction Freq:   %.4f\n", pll->output_vco_correction_freq);
    // printf("  VCO Applied Freq:      %.4f\n", pll->output_vco_applied_freq);
    // printf("  VCO Phase:             %.4f\n\n", pll->output_vco_phase);

    vco_controller_update(&pll->vco, pll->output_pi);
    pll->output_vco_correction_freq = pll->vco.freq_correction;
    pll->output_vco_applied_freq = pll->vco.frequency;
    pll->output_vco_phase = pll->vco.phase;

    float vco_cos = cosf(vco_controller_get_phase(&pll->vco));
    pd_update(&pll->phase_detector, 
              grid_voltage, 
              vco_cos, 
              &pll->output_phase_detector);

    pll_apply_notch_filter(pll, pll->output_phase_detector);
    // pll->output_notch_filter = pll->output_phase_detector;
    // pll->output_lpf = lpf_process(&pll->error_lpf, pll->output_notch_filter);
    pll->output_lpf = lpf_dyn_coeff_process(&pll->error_lpf, pll->output_notch_filter);
    pi_controller_update(&pll->pi_controller, pll->output_lpf);
    pll->output_pi = pll->pi_controller.control_signal;


    
    // Print values after update
    // printf("After PLL Update:\n");
    // printf("  Phase Detector Output: %.4f\n", pll->output_phase_detector);
    // printf("  Notch Filter Output:   %.4f\n", pll->output_notch_filter);
    // printf("  LPF Output:            %.4f\n", pll->output_lpf);
    // printf("  PI Output:             %.4f\n", pll->output_pi);
    // printf("  VCO Correction Freq:   %.4f\n", pll->output_vco_correction_freq);
    // printf("  VCO Applied Freq:      %.4f\n", pll->output_vco_applied_freq);
    // printf("  VCO Phase:             %.4f\n\n", pll->output_vco_phase);

    return PLL_SUCCESS;
}

void pll_apply_notch_filter(PLL* pll, float error_signal) {
    // Input validation
    if (!pll || pll->num_active_notches == 0) return;

      pll->output_notch_filter = error_signal;
    // Apply each active notch filter in sequence
    for (int i = 0; i < pll->num_active_notches; i++) {
         pll->output_notch_filter = notch_filter_apply(&pll->error_notch[i],  pll->output_notch_filter);
    }
}

