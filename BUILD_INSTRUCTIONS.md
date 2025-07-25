# Build Instructions for Letter Recognition System

## Issue Description
The system was showing "Loaded 0 templates" because there was a mismatch between the template file format and the loading function.

## What Was Fixed
1. **Template Format Mismatch**: The `template_generator` was creating a binary file (`templates.bin`), but `load_templates` was expecting a text format.
2. **Added Binary Template Loader**: Created `load_templates_binary()` function to properly read the binary template format.
3. **Fixed File Paths**: Updated paths in `template_generator.cpp` and `recognize.cpp` to work when running from `build_cpu` directory.

## Building on Linux/WSL (Ubuntu)

### Option 1: Linux Build Script (Recommended)
```bash
cd src
chmod +x build_linux.sh
./build_linux.sh
```

### Option 2: Simple Build Script (If Option 1 fails)
If you get SSE4.2 compilation errors, use this simpler build:
```bash
cd src
chmod +x build_linux_simple.sh
./build_linux_simple.sh
```

### Option 3: Manual Build
```bash
cd src
mkdir -p build_cpu
cd build_cpu

# Install dependencies if needed
sudo apt update
sudo apt install build-essential libopencv-dev pkg-config

# Compile
g++ -std=c++17 -O3 -fopenmp $(pkg-config --cflags opencv4) -o template_generator ../template_generator.cpp ../letter_recognition.cpp $(pkg-config --libs opencv4)
g++ -std=c++17 -O3 -fopenmp $(pkg-config --cflags opencv4) -o recognize ../recognize.cpp ../letter_recognition.cpp $(pkg-config --libs opencv4)
g++ -std=c++17 -O3 -fopenmp $(pkg-config --cflags opencv4) -o main ../main.cpp ../letter_recognition.cpp $(pkg-config --libs opencv4)
```

### Option 4: Test Paths First
If you're having path issues, test the paths first:
```bash
cd src
chmod +x build_test.sh
./build_test.sh
```

## Building on Windows

### Option 1: PowerShell (Recommended)
```powershell
cd src
.\build.ps1
```

### Option 2: Batch File
```cmd
cd src
build_simple.bat
```

### Option 3: Manual Build
If you have OpenCV installed, you can build manually:
```cmd
cd src
mkdir build_cpu
cd build_cpu
g++ -std=c++17 -O3 -I"<opencv_include_path>" -o template_generator.exe ../template_generator.cpp ../letter_recognition.cpp -L"<opencv_lib_path>" -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui
g++ -std=c++17 -O3 -I"<opencv_include_path>" -o recognize.exe ../recognize.cpp ../letter_recognition.cpp -L"<opencv_lib_path>" -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui
```

## Running the System

1. **Generate Templates** (from `src/build_cpu` directory):
   ```bash
   ./template_generator
   ```
   This will create `templates.bin` from the images in `dataset/`.

2. **Run Recognition** (from `src/build_cpu` directory):
   ```bash
   ./recognize
   ```
   This will test recognition on `test_images/test01.jpg`.

## Expected Output
After fixing the template loading issue, you should see:
```
Loaded X templates from templates.bin
Image: test01.jpg | Detected letter: <actual_letter>
```

## Troubleshooting

### OpenCV Not Found (Linux/WSL)
If you get OpenCV-related errors:
```bash
sudo apt update
sudo apt install libopencv-dev pkg-config
```

### Compiler Not Found (Linux/WSL)
If g++ is not found:
```bash
sudo apt update
sudo apt install build-essential
```

### Path Issues
If you get "No such file or directory" errors:
1. Run the path test: `./build_test.sh`
2. Check that you're running from the correct directory (`src/build_cpu`)
3. Verify the relative paths are correct for your setup

### OpenCV Not Found (Windows)
If you get OpenCV-related errors:
1. Install OpenCV via vcpkg: `vcpkg install opencv`
2. Or install via MSYS2: `pacman -S mingw-w64-x86_64-opencv`
3. Or download from opencv.org and update paths in build scripts

### Compiler Not Found (Windows)
If g++ is not found:
1. Install MinGW-w64
2. Add MinGW bin directory to PATH
3. Or use Visual Studio with MSVC compiler

### Template Generation Fails
If template generation fails:
1. Check that `dataset/` directory contains .jpg files
2. Verify file naming format: `letter_rotation_number.jpg`
3. Ensure images are readable

## File Structure After Build
```
src/build_cpu/
├── template_generator (or template_generator.exe)
├── recognize (or recognize.exe)
├── main (or main.exe)
└── templates.bin (generated)
``` 