#!/bin/bash

# Get the absolute path of the script's directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Add the library path to LD_LIBRARY_PATH
export LD_LIBRARY_PATH="$SCRIPT_DIR/lib_c:$LD_LIBRARY_PATH"

# Set CPATH to include lib_c directory for header files
export CPATH="$SCRIPT_DIR/lib_c:$CPATH"

# Print confirmation message
echo "Environment variables set:"
echo "LD_LIBRARY_PATH = $LD_LIBRARY_PATH"
echo "CPATH = $CPATH"