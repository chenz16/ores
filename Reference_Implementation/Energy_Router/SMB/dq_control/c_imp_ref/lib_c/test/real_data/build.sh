#!/bin/bash

echo "Current directory: $(pwd)"
echo "Building program..."

# Create build directory if it doesn't exist
mkdir -p build

# Clean everything first
rm -rf build/*

# Compile with assembly output preserved
echo "=== Compiling main program ==="
gcc -o build/process_data \
    main.c \
    ../../dq_transform/dq_transform_1phase.c \
    ../../beta_transform/beta_transform_1p.c \
    -I../../ \
    -lm

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "=== Running process_data ==="
    ./build/process_data
    echo "=== Now run: python3 plot_results.py ==="
else
    echo "Build failed!"
    exit 1
fi