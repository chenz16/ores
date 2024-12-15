#ifndef WRITE_IOS_H
#define WRITE_IOS_H

#include <stdint.h>
#include <stdbool.h>
#include "../table_def/table_def.h"
#include "../modulation_2pwm/modulation_2pwm.h"

// IO Address definitions for 4 H-bridges
#define PWM_OUTPUT_BASE_ADDR    0x43C00000
#define PWM_OUTPUT1_OFFSET      0x00000004
#define PWM_OUTPUT2_OFFSET      0x00000008
#define PWM_OUTPUT3_OFFSET      0x0000000C
#define PWM_OUTPUT4_OFFSET      0x00000010
#define PWM_OUTPUT5_OFFSET      0x00000014
#define PWM_OUTPUT6_OFFSET      0x00000018
#define PWM_OUTPUT7_OFFSET      0x0000001C  
#define PWM_OUTPUT8_OFFSET      0x00000020

// Add these definitions
#define PWM_BASE_ADDRESS 0x40000000  // Replace with your actual base address
#define PWM_OFFSET 0x1000            // Replace with your actual offset between PWM registers

// Function declarations
void initialize_pwm_ios(void);
void write_ios_from_dc_sources(single_dc_source_t * sorted_dc_sources, int num_of_modules);

#endif // WRITE_IOS_H