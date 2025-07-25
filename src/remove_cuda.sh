#!/bin/bash

# Script to remove CUDA packages and ensure CPU-only setup

set -e

echo "=== CUDA Removal Script ==="
echo "This script will remove CUDA-related packages and ensure CPU-only OpenCV"

if command -v pacman &> /dev/null; then
    echo "Detected Arch Linux (pacman)"
    
    echo "Removing CUDA packages..."
    sudo pacman -R cuda cudnn nvidia-cuda-toolkit --noconfirm 2>/dev/null || true
    
    echo "Removing OpenCV packages..."
    sudo pacman -R opencv opencv-cuda opencv-cuda-cudnn --noconfirm 2>/dev/null || true
    
    echo "Cleaning up dependencies..."
    sudo pacman -Rc protobuf qt6-base qt5-base --noconfirm 2>/dev/null || true
    
    echo "Installing CPU-only OpenCV..."
    sudo pacman -S opencv-core opencv-imgproc opencv-imgcodecs opencv-highgui --noconfirm
    
    echo "Verifying no CUDA packages remain..."
    if pacman -Q | grep -i cuda; then
        echo "WARNING: Some CUDA packages are still installed:"
        pacman -Q | grep -i cuda
    else
        echo "✓ No CUDA packages found"
    fi
    
elif command -v apt &> /dev/null; then
    echo "Detected Debian/Ubuntu (apt)"
    
    echo "Removing CUDA packages..."
    sudo apt remove --purge cuda* nvidia-cuda* -y
    sudo apt autoremove -y
    
    echo "Removing OpenCV packages..."
    sudo apt remove --purge libopencv* -y
    sudo apt autoremove -y
    
    echo "Installing CPU-only OpenCV..."
    sudo apt update
    sudo apt install libopencv-core-dev libopencv-imgproc-dev libopencv-imgcodecs-dev libopencv-highgui-dev --no-install-recommends
    
elif command -v dnf &> /dev/null; then
    echo "Detected Fedora/RHEL (dnf)"
    
    echo "Removing CUDA packages..."
    sudo dnf remove cuda* -y
    
    echo "Removing OpenCV packages..."
    sudo dnf remove opencv* -y
    
    echo "Installing CPU-only OpenCV..."
    sudo dnf install opencv-core-devel opencv-imgproc-devel opencv-imgcodecs-devel opencv-highgui-devel --setopt=install_weak_deps=0
    
else
    echo "Unsupported package manager. Please manually remove CUDA packages."
    exit 1
fi

echo ""
echo "CUDA removal completed!"
echo "You can now run: ./build_cpu_only.sh" 