cmake_minimum_required(VERSION 3.10)
project(LetterRecognition)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Base compiler flags
set(CMAKE_CXX_FLAGS "-O3 -fopenmp")

# Conditionally add ARM-specific flags
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a -mfpu=neon")
endif()

# Show architecture during configuration
message(STATUS "Target architecture: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Compiler flags: ${CMAKE_CXX_FLAGS}")

# Find OpenCV
find_package(OpenCV REQUIRED)

# Executable: template_generator
add_executable(template_generator template_generator.cpp letter_recognition.cpp)
target_link_libraries(template_generator ${OpenCV_LIBS})

# Executable: recognize
add_executable(recognize recognize.cpp letter_recognition.cpp)
target_link_libraries(recognize ${OpenCV_LIBS})
