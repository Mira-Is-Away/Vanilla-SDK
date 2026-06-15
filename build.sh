#!/bin/bash

# Exit on error
set -e

# Create build directory if it doesn't exist
mkdir -p bin

# Configure and build
cd bin
cmake ..
make -j"$(nproc)"

# Run the program (optional, might fail if dependencies are missing at runtime)
# ./vanilla