#!/bin/bash

echo "Current directory: $(pwd)"
echo "=== Compiling grid simulation ==="

# First compile the object files
gcc -c -Wall -I. -I../../ -D_USE_MATH_DEFINES \
    ../../log_data_rw/log_data_rw.c \
    ../../dq_controller_pid/dq_controller_pid.c \
    ../../dq_to_modulation/dq_to_modulation.c \
    ../../beta_transform/beta_transform_1p.c \
    ../../dq_transform/dq_transform_1phase.c \
    grid_simulation.c \
    plant_simulator.c \
    main.c

# Then link them together
gcc *.o -o controller_real_data -lm

if [ $? -eq 0 ]; then
    echo "Build successful!"
    ./controller_real_data
else
    echo "Build failed!"
    exit 1
fi

# Clean up object files
rm -f *.o
