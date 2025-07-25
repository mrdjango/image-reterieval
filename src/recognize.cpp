#include "letter_recognition.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

int main(int argc, char** argv) {
    // Load templates
    load_templates_binary("templates.bin");
    
    // Debug: Print template statistics
    debug_print_template_stats();
    
    // Load test image
    cv::Mat image = cv::imread("../../test_images/test01.jpg");
    if (image.empty()) {
        std::cerr << "Error: Could not load test image" << std::endl;
        return 1;
    }
    
    // Recognize letter
    char letter = recognize_letter(image);
    
    std::cout << "Image: test01.jpg | Detected letter: " << letter << "\n";
    return 0;
}