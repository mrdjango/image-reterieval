#include "letter_recognition.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

int main() {
    std::cout << "=== Letter Recognition Test ===" << std::endl;
    
    // Load templates
    try {
        load_templates_binary("templates.bin");
        std::cout << "Successfully loaded templates" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error loading templates: " << e.what() << std::endl;
        return 1;
    }
    
    // Debug: Print template statistics
    debug_print_template_stats();
    
    // Test with different thresholds
    std::vector<int> test_thresholds = {15, 30, 50, 100, 200};
    
    // Load test image
    cv::Mat image = cv::imread("../../test_images/test01.jpg");
    if (image.empty()) {
        std::cerr << "Error: Could not load test image" << std::endl;
        return 1;
    }
    
    std::cout << "Test image loaded: " << image.cols << "x" << image.rows << std::endl;
    
    std::cout << "\n=== Testing with different thresholds ===" << std::endl;
    
    for (int threshold : test_thresholds) {
        SAFE_THRESHOLD = threshold;
        std::cout << "\n--- Testing with threshold: " << threshold << " ---" << std::endl;
        
        char result = recognize_letter(image);
        std::cout << "Result: " << result << std::endl;
    }
    
    return 0;
} 