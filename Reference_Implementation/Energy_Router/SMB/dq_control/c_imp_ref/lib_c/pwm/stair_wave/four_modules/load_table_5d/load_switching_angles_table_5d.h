#ifndef LOAD_SWITCHING_ANGLES_TABLE_5D_H
#define LOAD_SWITCHING_ANGLES_TABLE_5D_H

#define N_POINTS_PER_CI 5   // Points per voltage ratio (0.95 to 1.05)
#define N_MOD_INDEX 6       // Points for modulation index (0.75 to 1.0)
#define NUM_DC_SOURCES 4    // Number of DC sources

typedef struct {
    float c1_values[N_POINTS_PER_CI];
    float c2_values[N_POINTS_PER_CI];
    float c3_values[N_POINTS_PER_CI];
    float c4_values[N_POINTS_PER_CI];
    float m_values[N_MOD_INDEX];
    float angles[N_POINTS_PER_CI][N_POINTS_PER_CI][N_POINTS_PER_CI][N_POINTS_PER_CI][N_MOD_INDEX][NUM_DC_SOURCES];
    float thd[N_POINTS_PER_CI][N_POINTS_PER_CI][N_POINTS_PER_CI][N_POINTS_PER_CI][N_MOD_INDEX];
    float v1_error[N_POINTS_PER_CI][N_POINTS_PER_CI][N_POINTS_PER_CI][N_POINTS_PER_CI][N_MOD_INDEX];
} SwitchingAnglesTable;

SwitchingAnglesTable* load_switching_angles_table_5d(void);
void free_switching_angles_table_5d(SwitchingAnglesTable* table);

#endif // LOAD_SWITCHING_ANGLES_TABLE_5D_H