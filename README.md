# Letter Recognition System

A computer vision system for recognizing letters from images, optimized for x86_64 Linux systems.

## Features

- **Cross-platform**: Optimized for both ARM (Raspberry Pi) and x86_64 (Desktop Linux) architectures
- **Hardware acceleration**: Uses SSE/AVX instructions on x86_64 and NEON on ARM
- **OpenCV integration**: Robust image processing capabilities
- **Template-based recognition**: Fast and accurate letter recognition
- **CPU-only operation**: No CUDA dependencies required

## System Requirements

### For x86_64 Linux Arch:
- GCC/G++ compiler
- CMake (3.10 or higher)
- OpenCV4 (CPU-only components only)
- OpenMP support

## Installation

### Option 1: CPU-Only Build (Recommended for most users)

1. Clone the repository:
```bash
git clone <repository-url>
cd image-reterieval
```

2. **If you have CUDA installed and want to remove it:**
```bash
cd src
chmod +x remove_cuda.sh
./remove_cuda.sh
```

3. Run the CPU-only build script:
```bash
chmod +x build_cpu_only.sh
./build_cpu_only.sh
```

### Option 2: Manual CUDA Removal (if needed)

If you encounter CUDA-related errors, manually remove CUDA packages:

```bash
# For Arch Linux
sudo pacman -R cuda cudnn opencv-cuda opencv-cuda-cudnn --noconfirm
sudo pacman -S opencv-core opencv-imgproc opencv-imgcodecs --noconfirm

# For Ubuntu/Debian
sudo apt remove --purge cuda* nvidia-cuda* libopencv* -y
sudo apt autoremove -y
sudo apt install libopencv-core-dev libopencv-imgproc-dev libopencv-imgcodecs-dev

# For Fedora/RHEL
sudo dnf remove cuda* opencv* -y
sudo dnf install opencv-core-devel opencv-imgproc-devel opencv-imgcodecs-devel
```

### Option 3: Manual Installation

1. Install dependencies:
```bash
# For Arch Linux (CPU-only)
sudo pacman -S opencv-core opencv-imgproc opencv-imgcodecs cmake gcc

# For Ubuntu/Debian
sudo apt update
sudo apt install libopencv-core-dev libopencv-imgproc-dev libopencv-imgcodecs-dev cmake build-essential

# For Fedora
sudo dnf install opencv-core-devel opencv-imgproc-devel opencv-imgcodecs-devel cmake gcc-c++
```

2. Build the project:
```bash
cd src
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Troubleshooting

### CUDA Version Mismatch Error

If you see an error like:
```
Could NOT find CUDAToolkit: Found unsuitable version "12.8.61", but required is exact version "12.9.86"
```

**Solution**: Remove CUDA and use CPU-only build:
```bash
cd src
./remove_cuda.sh
./build_cpu_only.sh
```

### OpenCV Linking Errors

If you encounter linking errors with protobuf, Qt, or other dependencies:

**Solution**: Install minimal OpenCV components:
```bash
# For Arch Linux
sudo pacman -R opencv opencv-cuda
sudo pacman -S opencv-core opencv-imgproc opencv-imgcodecs

# Or use the removal script
./remove_cuda.sh
```

### CUDA Packages Still Installed

To check if CUDA packages are still installed:
```bash
# Arch Linux
pacman -Q | grep -i cuda

# Ubuntu/Debian
dpkg -l | grep -i cuda

# Fedora/RHEL
rpm -qa | grep -i cuda
```

If any CUDA packages remain, remove them manually or use the removal script.

## Usage

### Main Application
```bash
./main <image_path> [templates_path]
```

Example:
```bash
./main ../test_images/test01.jpg ../templates/templates.txt
```

### Template Generator
```bash
./template_generator <dataset_path>
```

### Recognition Tool
```bash
./recognize <image_path>
```

## Architecture Optimizations

### x86_64 Optimizations
- **SSE4.2**: Used for bit counting operations
- **AVX2**: Vectorized operations where available
- **OpenMP**: Parallel processing support
- **Native architecture**: Compiler optimizations for your specific CPU

### ARM Optimizations (Raspberry Pi)
- **NEON**: ARM vector instructions
- **OpenGL ES**: Mobile GPU acceleration
- **ARMv8-A**: 64-bit ARM optimizations

## Project Structure

```
image-reterieval/
├── src/                    # Source code
│   ├── main.cpp           # Main application
│   ├── letter_recognition.cpp  # Core recognition logic
│   ├── letter_recognition.h    # Header file
│   ├── template_generator.cpp  # Template generation
│   ├── recognize.cpp      # Recognition tool
│   ├── CMakeLists.txt     # Build configuration
│   ├── build_and_run.sh   # Build script (CUDA-enabled)
│   ├── build_cpu_only.sh  # CPU-only build script
│   └── remove_cuda.sh     # CUDA removal script
├── dataset/               # Training dataset
├── templates/             # Generated templates
├── test_images/           # Test images
└── validation/            # Validation data
```

## Building for Different Architectures

The system automatically detects your architecture and applies appropriate optimizations:

- **x86_64**: Uses SSE/AVX instructions and standard OpenGL
- **ARM/aarch64**: Uses NEON instructions and OpenGL ES
- **Other**: Falls back to generic implementations

## Performance Tuning

For optimal performance on x86_64:

1. Enable all CPU features:
   ```bash
   export CFLAGS="-march=native -O3"
   export CXXFLAGS="-march=native -O3"
   ```

2. Use multiple cores for compilation:
   ```bash
   make -j$(nproc)
   ```

## License

This project is open source. Please check the license file for details.

## Contributing

Contributions are welcome! Please ensure your code works on both x86_64 and ARM architectures. 