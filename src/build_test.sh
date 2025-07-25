#!/bin/bash

echo "Building path test program..."

# Check if g++ is available
if ! command -v g++ &> /dev/null; then
    echo "Error: g++ not found. Please install build-essential:"
    echo "  sudo apt update && sudo apt install build-essential"
    exit 1
fi

# Create build directory
mkdir -p build_cpu
cd build_cpu

echo "Compiling test_paths..."
g++ -std=c++17 -o test_paths ../test_paths.cpp

if [ $? -eq 0 ]; then
    echo "Build successful! Running path test..."
    echo ""
    ./test_paths
else
    echo "Build failed!"
    exit 1
fi 