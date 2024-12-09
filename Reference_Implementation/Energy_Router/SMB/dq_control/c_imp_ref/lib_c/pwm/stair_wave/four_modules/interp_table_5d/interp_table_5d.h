#ifndef LOOKUP_TABLE_5D_H
#define LOOKUP_TABLE_5D_H

#include "load_switching_angles_table_5d.h"
#include "../../common/table_def/table_def.h"

#define MAX_ITERATIONS_FOR_ANGLE_CORRECTION 50

// Initialize the lookup table
// Returns: Pointer to initialized table on success, NULL on failure
const SwitchingAnglesTable* init_switching_angles_lookup_table(void);

// Free the lookup table
void cleanup_switching_angles_lookup_table(const SwitchingAnglesTable* table);

// Main lookup functions
void interpolate_switching_angles_5d(
    const SwitchingAnglesTable* table,
    single_dc_source_t* dc_sources,
    SwitchingAnglesResult* result
);

void interpolate_switching_angles_5d_with_correction(
    const SwitchingAnglesTable* table,
    single_dc_source_t* dc_sources,
    SwitchingAnglesResult* result
);

#endif // LOOKUP_TABLE_5D_H 