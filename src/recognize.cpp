#include "letter_recognition.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

int main(int argc, char** argv) {
    // Check command line arguments
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <image_path>" << std::endl;
        std::cerr << "Example: " << argv[0] << " test_images/test01.jpg" << std::endl;
        return 1;
    }
    
    std::string image_path = argv[1];
    
    // Load templates
    load_templates_binary("templates.bin");
    
    // Debug: Print template statistics
    debug_print_template_stats();
    
    // Load test image
    cv::Mat image = cv::imread(image_path);
    if (image.empty()) {
        std::cerr << "Error: Could not load test image: " << image_path << std::endl;
        return 1;
    }
    
    // Recognize letter with rotation
    RecognitionResult result = recognize_letter_with_rotation(image);
    
    std::cout << "\n=== Recognition Results ===" << std::endl;
    std::cout << "Image: " << image_path << std::endl;
    std::cout << "Detected letter: " << result.letter << std::endl;
    std::cout << "Detected rotation: " << result.rotation << "°" << std::endl;
    std::cout << "Confidence (hamming distance): " << result.confidence << std::endl;
    
    if (result.letter != '?') {
        std::cout << "✓ Letter recognized successfully!" << std::endl;
    } else {
        std::cout << "✗ Letter not recognized (confidence too low)" << std::endl;
    }
    
    return 0;
}