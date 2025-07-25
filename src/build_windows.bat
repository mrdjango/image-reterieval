@echo off
echo Building Letter Recognition System for Windows...

REM Check if g++ is available
where g++ >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: g++ not found. Please install MinGW or another C++ compiler.
    exit /b 1
)

REM Check if OpenCV is available
pkg-config --cflags opencv4 >nul 2>&1
if %errorlevel% neq 0 (
    echo Warning: OpenCV not found via pkg-config. Trying alternative paths...
    set OPENCV_INCLUDE=-I"C:/opencv/build/include"
    set OPENCV_LIBS=-L"C:/opencv/build/x64/mingw/lib" -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui
) else (
    set OPENCV_INCLUDE=$(pkg-config --cflags opencv4)
    set OPENCV_LIBS=$(pkg-config --libs opencv4)
)

REM Create build directory
if not exist build_cpu mkdir build_cpu
cd build_cpu

REM Compile template_generator
echo Compiling template_generator...
g++ -std=c++17 -O3 -fopenmp -march=native -msse4.2 -mavx2 %OPENCV_INCLUDE% -o template_generator.exe ../template_generator.cpp ../letter_recognition.cpp %OPENCV_LIBS%

REM Compile recognize
echo Compiling recognize...
g++ -std=c++17 -O3 -fopenmp -march=native -msse4.2 -mavx2 %OPENCV_INCLUDE% -o recognize.exe ../recognize.cpp ../letter_recognition.cpp %OPENCV_LIBS%

REM Compile main
echo Compiling main...
g++ -std=c++17 -O3 -fopenmp -march=native -msse4.2 -mavx2 %OPENCV_INCLUDE% -o main.exe ../main.cpp ../letter_recognition.cpp %OPENCV_LIBS%

REM Compile test program
echo Compiling test_recognition...
g++ -std=c++17 -O3 -fopenmp -march=native -msse4.2 -mavx2 %OPENCV_INCLUDE% -o test_recognition.exe ../test_recognition.cpp ../letter_recognition.cpp %OPENCV_LIBS%

echo Build completed!
echo.
echo Available executables:
echo   template_generator.exe - Generate letter templates
echo   recognize.exe - Recognize letters in images
echo   main.exe - Main application
echo   test_recognition.exe - Test recognition with debugging
echo.
echo To run the recognition system:
echo   recognize.exe
echo.
echo To test with debugging:
echo   test_recognition.exe
echo.
echo To generate templates:
echo   template_generator.exe 