@echo off
echo Building Letter Recognition System for Windows (Simple)...

REM Check if g++ is available
where g++ >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: g++ not found. Please install MinGW or another C++ compiler.
    exit /b 1
)

REM Create build directory
if not exist build_cpu mkdir build_cpu
cd build_cpu

REM Try different OpenCV paths
set OPENCV_FOUND=0

REM Try vcpkg path
if exist "C:/vcpkg/installed/x64-windows/include/opencv2" (
    echo Found OpenCV in vcpkg...
    set OPENCV_INCLUDE=-I"C:/vcpkg/installed/x64-windows/include"
    set OPENCV_LIBS=-L"C:/vcpkg/installed/x64-windows/lib" -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui
    set OPENCV_FOUND=1
)

REM Try standard OpenCV path
if %OPENCV_FOUND%==0 if exist "C:/opencv/build/include/opencv2" (
    echo Found OpenCV in standard path...
    set OPENCV_INCLUDE=-I"C:/opencv/build/include"
    set OPENCV_LIBS=-L"C:/opencv/build/x64/mingw/lib" -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui
    set OPENCV_FOUND=1
)

REM Try MSYS2 path
if %OPENCV_FOUND%==0 if exist "C:/msys64/mingw64/include/opencv4" (
    echo Found OpenCV in MSYS2...
    set OPENCV_INCLUDE=-I"C:/msys64/mingw64/include/opencv4"
    set OPENCV_LIBS=-L"C:/msys64/mingw64/lib" -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui
    set OPENCV_FOUND=1
)

if %OPENCV_FOUND%==0 (
    echo Error: OpenCV not found. Please install OpenCV and update the paths in this script.
    echo Common installation methods:
    echo   - vcpkg: vcpkg install opencv
    echo   - MSYS2: pacman -S mingw-w64-x86_64-opencv
    echo   - Manual download from opencv.org
    exit /b 1
)

REM Compile template_generator
echo Compiling template_generator...
g++ -std=c++17 -O3 %OPENCV_INCLUDE% -o template_generator.exe ../template_generator.cpp ../letter_recognition.cpp %OPENCV_LIBS%
if %errorlevel% neq 0 (
    echo Error compiling template_generator
    exit /b 1
)

REM Compile recognize
echo Compiling recognize...
g++ -std=c++17 -O3 %OPENCV_INCLUDE% -o recognize.exe ../recognize.cpp ../letter_recognition.cpp %OPENCV_LIBS%
if %errorlevel% neq 0 (
    echo Error compiling recognize
    exit /b 1
)

REM Compile main
echo Compiling main...
g++ -std=c++17 -O3 %OPENCV_INCLUDE% -o main.exe ../main.cpp ../letter_recognition.cpp %OPENCV_LIBS%
if %errorlevel% neq 0 (
    echo Error compiling main
    exit /b 1
)

echo Build completed successfully!
echo.
echo Available executables:
echo   template_generator.exe - Generate letter templates
echo   recognize.exe - Recognize letters in images
echo   main.exe - Main application
echo.
echo To run the recognition system:
echo   recognize.exe
echo.
echo To generate templates:
echo   template_generator.exe 