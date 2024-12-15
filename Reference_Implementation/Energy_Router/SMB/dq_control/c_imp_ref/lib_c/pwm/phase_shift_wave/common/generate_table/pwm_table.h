#ifndef PWM_TABLE_H
#define PWM_TABLE_H

#define N_INIT_PHASE 10
#define N_MOD_INDEX 10
#define N_GRID_POINTS 82

extern float init_phases[N_INIT_PHASE];
extern float mod_indices[N_MOD_INDEX];
extern float intersection_table[N_INIT_PHASE][N_MOD_INDEX][N_GRID_POINTS];

#endif // PWM_TABLE_H
