#include "letter_recognition.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

// Include the enhanced recognition function
#include "enhanced_recognition.cpp"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <image_path>" << std::endl;
        return 1;
    }
    
    std::string image_path = argv[1];
    
    // Load templates
    load_templates_binary("templates.bin");
    std::cout << "Loaded " << templates.size() << " templates" << std::endl;
    
    // Load test image
    cv::Mat image = cv::imread(image_path);
    if (image.empty()) {
        std::cerr << "Error: Could not load test image: " << image_path << std::endl;
        return 1;
    }
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "ORIGINAL RECOGNITION METHOD" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    RecognitionResult original_result = recognize_letter_with_rotation(image);
    
    std::cout << "\nOriginal Result:" << std::endl;
    std::cout << "  Letter: " << original_result.letter << std::endl;
    std::cout << "  Rotation: " << original_result.rotation << "°" << std::endl;
    std::cout << "  Confidence: " << original_result.confidence << std::endl;
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "ENHANCED RECOGNITION METHOD" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    RecognitionResult enhanced_result = recognize_letter_enhanced(image);
    
    std::cout << "\nEnhanced Result:" << std::endl;
    std::cout << "  Letter: " << enhanced_result.letter << std::endl;
    std::cout << "  Rotation: " << enhanced_result.rotation << "°" << std::endl;
    std::cout << "  Confidence: " << enhanced_result.confidence << std::endl;
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "COMPARISON" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    if (original_result.letter != enhanced_result.letter) {
        std::cout << "🔄 CHANGE: " << original_result.letter << " -> " << enhanced_result.letter << std::endl;
        if (enhanced_result.letter == 'a' && original_result.letter == 'o') {
            std::cout << "✅ FIXED: Corrected 'o' misclassification to 'a'" << std::endl;
        } else if (enhanced_result.letter == 'o' && original_result.letter == 'a') {
            std::cout << "⚠️  CHANGED: 'a' changed to 'o' (check if this is correct)" << std::endl;
        }
    } else {
        std::cout << "✓ SAME: Both methods detected '" << original_result.letter << "'" << std::endl;
    }
    
    std::cout << "Confidence improvement: " << (original_result.confidence - enhanced_result.confidence) << std::endl;
    
    return 0;
}