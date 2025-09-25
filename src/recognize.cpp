#include "letter_recognition.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    // Check command line arguments
    if (argc < 2 || argc > 5) {
        std::cerr << "Usage: " << argv[0] << " <image_path> [known_letter] [export_detected] [export_path]" << std::endl;
        std::cerr << "Examples:" << std::endl;
        std::cerr << "  " << argv[0] << " test_images/test01.jpg" << std::endl;
        std::cerr << "  " << argv[0] << " test_images/test01.jpg a" << std::endl;
        std::cerr << "  " << argv[0] << " test_images/test01.jpg a export_detected ./exported/" << std::endl;
        std::cerr << "  " << argv[0] << " test_images/test01.jpg ? export_detected ./exported/" << std::endl;
        return 1;
    }
    
    std::string image_path = argv[1];
    char known_letter = '?';
    bool export_detected = false;
    std::string export_path = "";
    
    // Parse known letter if provided
    if (argc >= 3) {
        std::string known_str = argv[2];
        if (known_str.length() == 1) {
            known_letter = known_str[0];
        } else if (known_str != "export_detected") {
            std::cerr << "Warning: Invalid known letter '" << known_str << "'. Using no hint." << std::endl;
        }
    }
    
    // Parse export options
    for (int i = 2; i < argc; i++) {
        if (std::string(argv[i]) == "export_detected") {
            export_detected = true;
            if (i + 1 < argc) {
                export_path = argv[i + 1];
                break;
            } else {
                std::cerr << "Error: export_detected specified but no export path provided." << std::endl;
                return 1;
            }
        }
    }
    
    // Validate export path if export is enabled
    if (export_detected) {
        if (export_path.empty()) {
            std::cerr << "Error: Export path cannot be empty when export_detected is specified." << std::endl;
            return 1;
        }
        
        // Create export directory if it doesn't exist
        try {
            fs::create_directories(export_path);
            std::cout << "Export enabled to: " << export_path << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error creating export directory: " << e.what() << std::endl;
            return 1;
        }
    }
    
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
    
    // Recognize letter with rotation and dynamic adjustment
    RecognitionResult result = recognize_letter_with_rotation(image, known_letter);
    
    std::cout << "\n=== Recognition Results ===" << std::endl;
    std::cout << "Image: " << image_path << std::endl;
    if (known_letter != '?') {
        std::cout << "Known letter hint: " << known_letter << std::endl;
    }
    std::cout << "Detected letter: " << result.letter << std::endl;
    std::cout << "Detected rotation: " << result.rotation << "°" << std::endl;
    std::cout << "Confidence (adjusted distance): " << result.confidence << std::endl;
    
    if (result.letter != '?') {
        std::cout << "✓ Letter recognized successfully!" << std::endl;
        if (known_letter != '?' && result.letter == known_letter) {
            std::cout << "✓ Matches expected letter!" << std::endl;
        } else if (known_letter != '?' && result.letter != known_letter) {
            std::cout << "⚠️  Does not match expected letter (" << known_letter << ")" << std::endl;
        }
        
        // Export functionality for successfully recognized letters
        if (export_detected) {
            try {
                // Create resized image (64x64) for export
                cv::Mat export_image;
                cv::resize(image, export_image, cv::Size(64, 64));
                
                // Generate filename: {letter detected}_{rotation}_2.jpg
                std::string export_filename = export_path + "/" + 
                                            std::string(1, result.letter) + "_" + 
                                            std::to_string(result.rotation) + "_2.jpg";
                
                // Save the resized image
                bool saved = cv::imwrite(export_filename, export_image);
                
                if (saved) {
                    std::cout << "✓ Exported recognized image to: " << export_filename << std::endl;
                    std::cout << "  - Letter: " << result.letter << std::endl;
                    std::cout << "  - Rotation: " << result.rotation << "°" << std::endl;
                    std::cout << "  - Size: 64x64" << std::endl;
                    std::cout << "  - Confidence: " << result.confidence << std::endl;
                } else {
                    std::cerr << "✗ Failed to export image to: " << export_filename << std::endl;
                }
                
            } catch (const std::exception& e) {
                std::cerr << "Error during export: " << e.what() << std::endl;
            }
        }
    } else {
        std::cout << "✗ Letter not recognized (confidence too low)" << std::endl;
        if (export_detected) {
            std::cout << "ℹ️  Image not exported (recognition failed)" << std::endl;
        }
    }
    
    return 0;
}