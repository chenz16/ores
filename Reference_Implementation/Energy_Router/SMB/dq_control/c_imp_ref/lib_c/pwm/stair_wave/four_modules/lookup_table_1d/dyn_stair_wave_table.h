#ifndef DYN_STAIR_WAVE_TABLE_H
#define DYN_STAIR_WAVE_TABLE_H

#include <stdbool.h>
#include "../../common/table_def/table_def.h"
#include "../interp_table_5d/interp_table_5d.h"

// Update an existing 1D switching table based on given parameters
void update_stair_wave_table(StairWaveTable* table,
                            SwitchingAnglesTable* angles_table, 
                            single_dc_source_t* dc_sources, 
                            const int num_modules);

// Free the allocated memory for the switching table
void free_stair_wave_table(StairWaveTable* table);

// Get switching state and index for any angle using binary search
SwitchingStateResult get_switching_state(const StairWaveTable* table, 
                                        float angle, 
                                        float angle_safe_margin, 
                                        bool force_reset);

// Initialize a new stair wave table with memory allocation
StairWaveTable* init_stair_wave_table(const int num_modules);

#endif