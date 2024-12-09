#ifndef CONTROL_IO_PORTS_H
#define CONTROL_IO_PORTS_H

#include "../common/modulation_2pwm/modulation_2pwm.h"
#include "../lookup_table_1d/dyn_1d_table.h"
#include "../load_table_5d/load_switching_angles_table_5d.h"
#include "../lookup_table_5d/lookup_table_5d.h"
#include "../../common/table_def/table_def.h"
#include <stdbool.h>

#define NUM_OF_MODULES 4
#define MAX_NUM_MODULES 4

typedef struct {
    SwitchingAnglesTable* switching_angles_5d;  // static 5D lookup table
    DynamicTable1D* switching_table_1d;       // Dynamic 1D table
    bool initialized;                           // Initialization status
} PWMTable;

PWMTable* init_pwm_control(const char* table_path);
bool control_io_ports(
    PWMTable* table,
    single_dc_source_t * dc_sources,
    float angle_at_t,
    bool update_table
);

void free_pwm_control(PWMTable* table);


#endif // PWM_CONTROL_H
