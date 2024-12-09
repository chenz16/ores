#include "control_io_ports.h"
#include "../modulation_2pwm/modulation_2pwm.h"
#include <stdlib.h>
#include <math.h>

PWMTable* init_pwm_control(const char* table_path) {
    // Allocate table structure
    PWMTable* table = (PWMTable*)malloc(sizeof(PWMTable));
    if (!table) return NULL;
    
    // Initialize structure
    table->switching_angles_5d = NULL;
    table->switching_table_1d = NULL;
    table->initialized = false;
    
    // Load 5D switching angles table
    table->switching_angles_5d = load_switching_angles_table(table_path);
    if (!table->switching_angles_5d) {
        free(table);
        return NULL;
    }
    
    initialize_pwm_ios();
    table->initialized = true;
    return table;
}


bool control_io_ports(
    PWMTable* table,
    single_dc_source_t * sorted_dc_sources,
    float angle_at_t,
    bool update_table
) {
    if (!table || !table->initialized || !sorted_dc_sources) {
        return false;
    }
    
    if (!table->switching_table_1d || update_table) {
        table->switching_table_1d = create_switching_table_1d(
            sorted_dc_sources[0].c, sorted_dc_sources[1].c, 
            sorted_dc_sources[2].c, sorted_dc_sources[3].c, 
            sorted_dc_sources[0].m_common
        );
        if (!table->switching_table_1d) {
            return false;
        }   
    } 

    SwitchingStateResult state = get_switching_state(
        table->switching_table_1d,
        angle_at_t,
        true
    );

    convert_system_state_to_module_state(sorted_dc_sources, &state, NUM_OF_MODULES);
    write_ios_from_dc_sources(sorted_dc_sources, NUM_OF_MODULES);
    return true;
}


void free_pwm_control(PWMTable* table) {
    if (table) {
        if (table->switching_angles_5d) {
            free_switching_angles_table(table->switching_angles_5d);
        }
        if (table->switching_table_1d) {
            free_switching_table_1d(table->switching_table_1d);
        }
        free(table);
    }
}
