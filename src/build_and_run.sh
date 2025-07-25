#!/bin/bash

# Build and run script for x86_64 Linux Arch
# Letter Recognition System

set -e  # Exit on any error

echo "=== Letter Recognition System Build Script ==="
echo "Target: x86_64 Linux Arch"

# Check if we're on x86_64
if [[ $(uname -m) != "x86_64" ]]; then
    echo "Warning: This script is optimized for x86_64 architecture"
    echo "Current architecture: $(uname -m)"
fi

# Check for required packages
echo "Checking dependencies..."

# Check for OpenCV
if ! pkg-config --exists opencv4; then
    echo "OpenCV4 not found. Installing..."
    if command -v pacman &> /dev/null; then
        sudo pacman -S opencv
    elif command -v apt &> /dev/null; then
        sudo apt update && sudo apt install libopencv-dev
    elif command -v dnf &> /dev/null; then
        sudo dnf install opencv-devel
    else
        echo "Please install OpenCV4 manually for your distribution"
        exit 1
    fi
fi

# Check for build tools
if ! command -v cmake &> /dev/null; then
    echo "CMake not found. Installing..."
    if command -v pacman &> /dev/null; then
        sudo pacman -S cmake
    elif command -v apt &> /dev/null; then
        sudo apt install cmake
    elif command -v dnf &> /dev/null; then
        sudo dnf install cmake
    else
        echo "Please install CMake manually"
        exit 1
    fi
fi

if ! command -v g++ &> /dev/null; then
    echo "G++ not found. Installing..."
    if command -v pacman &> /dev/null; then
        sudo pacman -S gcc
    elif command -v apt &> /dev/null; then
        sudo apt install build-essential
    elif command -v dnf &> /dev/null; then
        sudo dnf install gcc-c++
    else
        echo "Please install GCC/G++ manually"
        exit 1
    fi
fi

echo "Dependencies OK!"

# Create build directory
echo "Creating build directory..."
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
echo "Building..."
make -j$(nproc)

echo "Build completed successfully!"
echo ""
echo "Available executables:"
echo "  ./template_generator - Generate letter templates"
echo "  ./recognize - Recognize letters in images"
echo "  ./main - Main application"
echo ""
echo "To run the recognition system:"
echo "  ./main <image_path>"
echo ""
echo "To generate templates:"
echo "  ./template_generator <dataset_path>"
