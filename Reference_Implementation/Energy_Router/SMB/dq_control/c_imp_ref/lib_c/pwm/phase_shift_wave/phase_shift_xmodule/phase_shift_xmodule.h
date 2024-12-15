#ifndef PHASE_SHIFT_XMODULE_H
#define PHASE_SHIFT_XMODULE_H

#include "../common/pwm_table_loader/pwm_table_loader.h"
#include <stdint.h>

typedef struct {
    int8_t* module_status;  // Array of status values (-1, 0, 1)
    uint32_t num_modules;   // Number of modules
} MMCPWMStatus;

/**
 * Get PWM status for all MMC modules and calculate next interrupt time
 * 
 * @param tables Pointer to loaded PWM tables
 * @param module_init_phases Array of initial phases for each module
 * @param num_modules Number of modules
 * @param mod_index Modulation index (0.0 to 1.0)
 * @param current_phase Current phase angle (0 to 2π)
 * @param next_interrupt_time Output parameter for time until next state change
 * @return MMCPWMStatus structure containing status for all modules
 */
MMCPWMStatus get_mmc_pwm_status(const PWMTables* tables,
                               const float* module_init_phases,
                               uint32_t num_modules,
                               float mod_index,
                               float current_phase,
                               float* next_interrupt_time);

/**
 * Free resources allocated for MMCPWMStatus
 * 
 * @param status Pointer to MMCPWMStatus structure to free
 */
void free_mmc_pwm_status(MMCPWMStatus* status);

#endif // PHASE_SHIFT_XMODULE_H
