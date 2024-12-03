#!/bin/bash

echo "Current directory: $(pwd)"
echo "Building program..."

# Clean up previous build
rm -rf obj
rm -f dq_decomp_real_data
rm -f dq_to_modulation.*

# Copy required files locally
cp ../../dq_to_modulation/dq_to_modulation.* .

# Create object files directory
mkdir -p obj

# Compile each file
gcc -c dq_to_modulation.c -I. -o obj/dq_to_modulation.o
gcc -c ../../beta_transform/beta_transform_1p.c -I../../ -o obj/beta_transform_1p.o
gcc -c ../../dq_transform/dq_transform_1phase.c -I../../ -o obj/dq_transform_1phase.o
gcc -c ../../log_data_rw/log_data_rw.c -I../../ -o obj/log_data_rw.o
gcc -c main.c -I. -I../../ -o obj/main.o

# Link all object files
gcc obj/beta_transform_1p.o obj/dq_transform_1phase.o obj/dq_to_modulation.o obj/log_data_rw.o obj/main.o -o dq_decomp_real_data -lm

if [ $? -eq 0 ]; then
    echo "Build successful!"
    ./dq_decomp_real_data
    
    # Plot using Python (if available)
    if command -v python3 &> /dev/null; then
        python3 plot_results.py
    else
        echo "Python not found. Please plot log_data_with_beta.csv manually."
    fi
else
    echo "Build failed!"
    exit 1
fi