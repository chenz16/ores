#include "write_ios.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

// Default to test mode unless explicitly set to REAL_MODE
#ifndef REAL_MODE
    #define PWM_MODE_TEST
#endif

static volatile uint32_t** pwm_outputs = NULL;
#ifdef PWM_MODE_TEST
    static uint32_t* pwm_memory = NULL;
#endif
static bool pwm_initialized = false;

// Forward declaration of static function
static void cleanup_pwm_outputs(void);

static bool initialize_pwm_outputs(void) {
    if (pwm_initialized) return true;
    
    if (MAX_NUM_MODULES <= 0 || MAX_NUM_MODULES > 4) {
        return false;
    }
    
    pwm_outputs = (volatile uint32_t**)malloc(MAX_NUM_MODULES * sizeof(volatile uint32_t*));
    if (!pwm_outputs) return false;

#ifdef PWM_MODE_TEST
    // Test mode: Allocate memory for simulation
    pwm_memory = (uint32_t*)malloc(MAX_NUM_MODULES * sizeof(uint32_t));
    if (!pwm_memory) {
        free(pwm_outputs);
        pwm_outputs = NULL;
        return false;
    }
    
    for (int i = 0; i < MAX_NUM_MODULES; i++) {
        pwm_memory[i] = 0;
        pwm_outputs[i] = (volatile uint32_t*)&pwm_memory[i];
    }
#else
    // Real mode: Use actual hardware addresses for 4 H-bridges
    pwm_outputs[0] = (volatile uint32_t*)(PWM_OUTPUT_BASE_ADDR + PWM_OUTPUT1_OFFSET);
    pwm_outputs[1] = (volatile uint32_t*)(PWM_OUTPUT_BASE_ADDR + PWM_OUTPUT2_OFFSET);
    pwm_outputs[2] = (volatile uint32_t*)(PWM_OUTPUT_BASE_ADDR + PWM_OUTPUT3_OFFSET);
    pwm_outputs[3] = (volatile uint32_t*)(PWM_OUTPUT_BASE_ADDR + PWM_OUTPUT4_OFFSET);
#endif

    pwm_initialized = true;
    return true;
}

static void cleanup_pwm_outputs(void) {
#ifdef PWM_MODE_TEST
    if (pwm_memory) {
        free(pwm_memory);
        pwm_memory = NULL;
    }
#endif
    if (pwm_outputs) {
        free(pwm_outputs);
        pwm_outputs = NULL;
    }
    pwm_initialized = false;
}

void write_ios_from_dc_sources(single_dc_source_t* dc_sources, int num_of_modules) {
    if (!dc_sources || num_of_modules <= 0 || num_of_modules > MAX_NUM_MODULES) {
        return;
    }
    
    if (!initialize_pwm_outputs()) {
        return;
    }
    
    // Add debug prints
    printf("\nWriting PWM states:\n");
    for (int i = 0; i < num_of_modules; i++) {
        int io_index = dc_sources[i].io_index;
        if (io_index >= 0 && io_index < MAX_NUM_MODULES && pwm_outputs[io_index]) {
            uint32_t state = convert_HBridgeState_to_binary(dc_sources[i].pwm_state);
            *pwm_outputs[io_index] = state;
            printf("Module %d (io_index %d): State=%d, Binary=0x%x\n", 
                   i, io_index, dc_sources[i].pwm_state, state);
        }
    }

#ifdef PWM_MODE_TEST
    // Verify what was actually written
    printf("\nVerifying PWM memory:\n");
    for (int i = 0; i < num_of_modules; i++) {
        printf("Memory[%d] = 0x%x\n", i, pwm_memory[i]);
    }
#endif
}

void cleanup_pwm_ios(void) {
    cleanup_pwm_outputs();
}