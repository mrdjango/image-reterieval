#include "letter_recognition.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <vector>

namespace fs = std::filesystem;

int main() {
    // Create output file
    std::ofstream out("templates.bin", std::ios::binary);
    
    // Process all images in dataset directory
    for (const auto& entry : fs::directory_iterator("../../dataset")) {
        if (entry.path().extension() != ".jpg") continue;
        
        // Parse filename
        std::string filename = entry.path().stem().string();
        
        // Find the first underscore to get the letter
        size_t first_underscore = filename.find('_');
        if (first_underscore == std::string::npos) {
            std::cerr << "Warning: Skipping file with invalid format: " << filename << std::endl;
            continue;
        }
        
        // Extract letter (could be multiple characters like "div", "mul", etc.)
        std::string letter_str = filename.substr(0, first_underscore);
        char letter = letter_str[0]; // Use first character as the main letter
        
        // Find the second underscore to get the rotation
        size_t second_underscore = filename.find('_', first_underscore + 1);
        if (second_underscore == std::string::npos) {
            std::cerr << "Warning: Skipping file with invalid format: " << filename << std::endl;
            continue;
        }
        
        // Extract rotation number
        std::string rotation_str = filename.substr(first_underscore + 1, second_underscore - first_underscore - 1);
        int rotation;
        try {
            rotation = std::stoi(rotation_str);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Warning: Invalid rotation number in file: " << filename << " (rotation: " << rotation_str << ")" << std::endl;
            continue;
        }
        
        // Load image (already cropped)
        cv::Mat img = cv::imread(entry.path().string());
        if (img.empty()) continue;
        
        // Resize to standard size (64x64)
        cv::Mat resized;
        cv::resize(img, resized, cv::Size(64, 64));
        
        // Process image
        cv::Mat binary;
        adaptive_binarize(resized, binary);
        
        // Generate 9 shifted versions (±1 pixel tolerance)
        /*
        for (int dx : {-1, 0, 1}) {
            for (int dy : {-1, 0, 1}) {
                Template t;
                t.letter = letter;
                t.rotation = rotation;
                
                // Shift and pack
                center_and_pack(binary, t.bits);
                
                // Save to file
                out.write(&t.letter, 1);
                out.write(reinterpret_cast<char*>(&t.rotation), sizeof(int));
                out.write(reinterpret_cast<char*>(t.bits.data()), 512);  // 512 bytes for 64x64
            }
        }
        */
        
        // Generate single unshifted version
        Template t;
        t.letter = letter;
        t.rotation = rotation;
        
        // Pack without shifting
        center_and_pack(binary, t.bits);
        
        // Save to file
        out.write(&t.letter, 1);
        out.write(reinterpret_cast<char*>(&t.rotation), sizeof(int));
        out.write(reinterpret_cast<char*>(t.bits.data()), 512);  // 512 bytes for 64x64
    }
    return 0;
}