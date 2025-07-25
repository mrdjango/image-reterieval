cmake_minimum_required(VERSION 3.10)
project(LetterRecognition)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Base compiler flags
set(CMAKE_CXX_FLAGS "-O3 -fopenmp")

# Platform-specific compiler flags
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    # ARM-specific flags for Raspberry Pi
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a -mfpu=neon")
    message(STATUS "Configuring for ARM architecture")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    # x86_64-specific flags for desktop Linux
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -msse4.2 -mavx2")
    message(STATUS "Configuring for x86_64 architecture")
else()
    message(STATUS "Using generic compiler flags")
endif()

# Show architecture during configuration
message(STATUS "Target architecture: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Compiler flags: ${CMAKE_CXX_FLAGS}")

# Find OpenCV headers
find_path(OpenCV_INCLUDE_DIRS
    NAMES opencv2/opencv.hpp
    PATHS /usr/include /usr/local/include
    PATH_SUFFIXES opencv4 opencv
)

if(NOT OpenCV_INCLUDE_DIRS)
    message(FATAL_ERROR "OpenCV headers not found. Please install opencv-core, opencv-imgproc, opencv-imgcodecs, and opencv-highgui")
endif()

# Find specific OpenCV libraries manually
find_library(OpenCV_core_LIBRARY
    NAMES opencv_core
    PATHS /usr/lib /usr/local/lib
    PATH_SUFFIXES x86_64-linux-gnu
)

find_library(OpenCV_imgproc_LIBRARY
    NAMES opencv_imgproc
    PATHS /usr/lib /usr/local/lib
    PATH_SUFFIXES x86_64-linux-gnu
)

find_library(OpenCV_imgcodecs_LIBRARY
    NAMES opencv_imgcodecs
    PATHS /usr/lib /usr/local/lib
    PATH_SUFFIXES x86_64-linux-gnu
)

find_library(OpenCV_highgui_LIBRARY
    NAMES opencv_highgui
    PATHS /usr/lib /usr/local/lib
    PATH_SUFFIXES x86_64-linux-gnu
)

if(NOT OpenCV_core_LIBRARY OR NOT OpenCV_imgproc_LIBRARY OR NOT OpenCV_imgcodecs_LIBRARY OR NOT OpenCV_highgui_LIBRARY)
    message(FATAL_ERROR "Required OpenCV libraries not found. Please install opencv-core, opencv-imgproc, opencv-imgcodecs, and opencv-highgui")
endif()

message(STATUS "Found OpenCV libraries:")
message(STATUS "  core: ${OpenCV_core_LIBRARY}")
message(STATUS "  imgproc: ${OpenCV_imgproc_LIBRARY}")
message(STATUS "  imgcodecs: ${OpenCV_imgcodecs_LIBRARY}")
message(STATUS "  highgui: ${OpenCV_highgui_LIBRARY}")

# Create a custom target with only the libraries we need
add_library(opencv_minimal INTERFACE)
target_include_directories(opencv_minimal INTERFACE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(opencv_minimal INTERFACE 
    ${OpenCV_core_LIBRARY}
    ${OpenCV_imgproc_LIBRARY}
    ${OpenCV_imgcodecs_LIBRARY}
    ${OpenCV_highgui_LIBRARY}
)

# Executable: template_generator
add_executable(template_generator template_generator.cpp letter_recognition.cpp)
target_link_libraries(template_generator opencv_minimal)

# Executable: recognize
add_executable(recognize recognize.cpp letter_recognition.cpp)
target_link_libraries(recognize opencv_minimal)

# Executable: main
add_executable(main main.cpp letter_recognition.cpp)
target_link_libraries(main opencv_minimal)
