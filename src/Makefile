# Makefile for Letter Recognition System
# Optimized for x86_64 Linux Arch

CXX = g++
CXXFLAGS = -std=c++17 -O3 -fopenmp -march=native -msse4.2 -mavx2
INCLUDES = $(shell pkg-config --cflags opencv4)
LIBS = $(shell pkg-config --libs opencv4)

# Source files
LETTER_RECOGNITION_SRC = letter_recognition.cpp
TEMPLATE_GENERATOR_SRC = template_generator.cpp $(LETTER_RECOGNITION_SRC)
RECOGNIZE_SRC = recognize.cpp $(LETTER_RECOGNITION_SRC)
MAIN_SRC = main.cpp $(LETTER_RECOGNITION_SRC)

# Targets
all: template_generator recognize main

template_generator: $(TEMPLATE_GENERATOR_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

recognize: $(RECOGNIZE_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

main: $(MAIN_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

clean:
	rm -f template_generator recognize main

install-deps:
	@echo "Installing dependencies for Arch Linux..."
	@if command -v pacman >/dev/null 2>&1; then \
		sudo pacman -S opencv cmake gcc; \
	elif command -v apt >/dev/null 2>&1; then \
		sudo apt update && sudo apt install libopencv-dev cmake build-essential; \
	elif command -v dnf >/dev/null 2>&1; then \
		sudo dnf install opencv-devel cmake gcc-c++; \
	else \
		echo "Please install dependencies manually for your distribution"; \
	fi

.PHONY: all clean install-deps 