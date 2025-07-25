#!/bin/bash

echo "Building Letter Recognition System for Linux..."

# Check if g++ is available
if ! command -v g++ &> /dev/null; then
    echo "Error: g++ not found. Please install build-essential:"
    echo "  sudo apt update && sudo apt install build-essential"
    exit 1
fi

# Check if OpenCV is available
if ! pkg-config --exists opencv4; then
    echo "OpenCV not found. Installing OpenCV..."
    sudo apt update
    sudo apt install libopencv-dev pkg-config
fi

# Create build directory
mkdir -p build_cpu
cd build_cpu

# Get OpenCV flags
OPENCV_CFLAGS=$(pkg-config --cflags opencv4)
OPENCV_LIBS=$(pkg-config --libs opencv4)

echo "Compiling template_generator..."
g++ -std=c++17 -O3 -fopenmp -msse4.2 -mavx2 $OPENCV_CFLAGS -o template_generator ../template_generator.cpp ../letter_recognition.cpp $OPENCV_LIBS
if [ $? -ne 0 ]; then
    echo "Error compiling template_generator"
    exit 1
fi

echo "Compiling recognize..."
g++ -std=c++17 -O3 -fopenmp -msse4.2 -mavx2 $OPENCV_CFLAGS -o recognize ../recognize.cpp ../letter_recognition.cpp $OPENCV_LIBS
if [ $? -ne 0 ]; then
    echo "Error compiling recognize"
    exit 1
fi

echo "Compiling main..."
g++ -std=c++17 -O3 -fopenmp -msse4.2 -mavx2 $OPENCV_CFLAGS -o main ../main.cpp ../letter_recognition.cpp $OPENCV_LIBS
if [ $? -ne 0 ]; then
    echo "Error compiling main"
    exit 1
fi

echo "Build completed successfully!"
echo ""
echo "Available executables:"
echo "  ./template_generator - Generate letter templates"
echo "  ./recognize - Recognize letters in images"
echo "  ./main - Main application"
echo ""
echo "To run the recognition system:"
echo "  ./recognize"
echo ""
echo "To generate templates:"
echo "  ./template_generator" 