#!/bin/bash

# CPU-only build script for Letter Recognition System
# This bypasses CUDA version compatibility issues

set -e  # Exit on any error

echo "=== Letter Recognition System CPU-Only Build Script ==="
echo "Target: x86_64 Linux Arch (CPU-only)"

# Check if we're on x86_64
if [[ $(uname -m) != "x86_64" ]]; then
    echo "Warning: This script is optimized for x86_64 architecture"
    echo "Current architecture: $(uname -m)"
fi

# Check for required packages
echo "Checking dependencies..."

# Remove any problematic OpenCV packages first
if command -v pacman &> /dev/null; then
    echo "Removing any existing OpenCV packages to avoid conflicts..."
    sudo pacman -R opencv opencv-cuda opencv-cuda-cudnn --noconfirm 2>/dev/null || true
    
    echo "Removing CUDA-related packages..."
    sudo pacman -R cuda cudnn --noconfirm 2>/dev/null || true
    
    echo "Cleaning up any remaining OpenCV dependencies..."
    sudo pacman -Rc protobuf qt6-base qt5-base --noconfirm 2>/dev/null || true
    
    echo "Installing minimal OpenCV components (CPU-only)..."
    sudo pacman -S opencv-core opencv-imgproc opencv-imgcodecs opencv-highgui --noconfirm
    
    echo "Verifying installation..."
    if pacman -Q opencv-cuda &> /dev/null; then
        echo "ERROR: opencv-cuda is still installed. Removing it..."
        sudo pacman -R opencv-cuda --noconfirm
    fi
    
elif command -v apt &> /dev/null; then
    echo "Removing CUDA and OpenCV packages..."
    # sudo apt remove --purge libopencv* cuda* nvidia-cuda* -y
    # sudo apt autoremove -y
    
    echo "Installing minimal OpenCV components..."
    sudo apt update
    sudo apt install libopencv-core-dev libopencv-imgproc-dev libopencv-imgcodecs-dev libopencv-highgui-dev --no-install-recommends
elif command -v dnf &> /dev/null; then
    echo "Removing CUDA and OpenCV packages..."
    sudo dnf remove cuda* opencv* -y
    
    echo "Installing minimal OpenCV components..."
    sudo dnf install opencv-core-devel opencv-imgproc-devel opencv-imgcodecs-devel opencv-highgui-devel --setopt=install_weak_deps=0
else
    echo "Please install OpenCV manually for your distribution"
    echo "Required packages: opencv-core, opencv-imgproc, opencv-imgcodecs, opencv-highgui"
    echo "Make sure to avoid CUDA-enabled versions"
    exit 1
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
mkdir -p build_cpu
cd build_cpu

# Configure with CMake for CPU-only build
echo "Configuring with CMake (CPU-only)..."
cmake .. -DCMAKE_BUILD_TYPE=Release -DOPENCV_ENABLE_NONFREE=OFF

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