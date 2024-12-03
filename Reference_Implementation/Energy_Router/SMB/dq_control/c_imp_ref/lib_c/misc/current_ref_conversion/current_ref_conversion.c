
#include "current_ref_conversion.h"

int32_t convert_power_to_dq_current_ref(
    float power_ref_w,
    float power_factor,
    float grid_voltage_d,
    float grid_voltage_q,
    float* current_ref_d,
    float* current_ref_q)
{
    // Input validation
    if (current_ref_d == NULL || current_ref_q == NULL) {
        return -1;
    }
    
    if (power_factor < 0.0f || power_factor > 1.0f) {
        return -2;
    }

    // Calculate magnitude of grid voltage
    float v_magnitude = sqrtf(grid_voltage_d * grid_voltage_d + 
                            grid_voltage_q * grid_voltage_q);
    
    if (v_magnitude < 1.0f) {
        *current_ref_d = 0.0f;
        *current_ref_q = 0.0f;
        return -3;
    }

    // Calculate current magnitude from power
    float current_magnitude = fabsf(power_ref_w) / v_magnitude;
    
    if (power_ref_w >= 0.0f) {  // Charging mode (power flows from grid to device)
        // Calculate active and reactive components for charging
        float i_active = current_magnitude * power_factor;
        float i_reactive = current_magnitude * sqrtf(1.0f - power_factor * power_factor);
        
        // Convention: negative Q for capacitive current in charging mode
        i_reactive = -i_reactive;
        
        // Convert to dq frame for charging
        *current_ref_d = i_active * (grid_voltage_d / v_magnitude) - 
                        i_reactive * (grid_voltage_q / v_magnitude);
        *current_ref_q = i_active * (grid_voltage_q / v_magnitude) + 
                        i_reactive * (grid_voltage_d / v_magnitude);
    }
    else {  // Discharging mode (power flows from device to grid)
        // Calculate active and reactive components for discharging
        float i_active = current_magnitude * power_factor;
        float i_reactive = current_magnitude * sqrtf(1.0f - power_factor * power_factor);
        
        // Convention: positive Q for inductive current in discharging mode
        // Convert to dq frame for discharging (note the negative signs)
        *current_ref_d = -(i_active * (grid_voltage_d / v_magnitude) - 
                          i_reactive * (grid_voltage_q / v_magnitude));
        *current_ref_q = -(i_active * (grid_voltage_q / v_magnitude) + 
                          i_reactive * (grid_voltage_d / v_magnitude));
    }
    
    return 0;  // Success
}
