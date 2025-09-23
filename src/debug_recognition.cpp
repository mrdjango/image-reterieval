#include "letter_recognition.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <image_path>" << std::endl;
        return 1;
    }
    
    std::string image_path = argv[1];
    
    // Load templates
    load_templates_binary("templates.bin");
    
    // Load test image
    cv::Mat image = cv::imread(image_path);
    if (image.empty()) {
        std::cerr << "Error: Could not load test image: " << image_path << std::endl;
        return 1;
    }
    
    // Test with different thresholds
    std::vector<int> thresholds = {100, 300, 500, 700, 1000};
    
    for (int threshold : thresholds) {
        SAFE_THRESHOLD = threshold;
        std::cout << "\n=== Testing with threshold: " << threshold << " ===" << std::endl;
        
        RecognitionResult result = recognize_letter_with_rotation(image);
        
        std::cout << "Result: " << result.letter << " (rotation: " << result.rotation << "°)" << std::endl;
        std::cout << "Confidence: " << result.confidence << std::endl;
        
        if (result.letter != '?') {
            std::cout << "✓ Recognized!" << std::endl;
        } else {
            std::cout << "✗ Not recognized (threshold too high)" << std::endl;
        }
    }
    
    return 0;
}