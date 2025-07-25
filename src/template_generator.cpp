#include "letter_recognition.h"
#include <filesystem>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <vector>

namespace fs = std::filesystem;

int main() {
    // Create output file
    std::ofstream out("templates.bin", std::ios::binary);
    
    // Process all images in dataset directory
    for (const auto& entry : fs::directory_iterator("dataset")) {
        if (entry.path().extension() != ".jpg") continue;
        
        // Parse filename
        std::string filename = entry.path().stem().string();
        char letter = filename[0];
        int rotation = std::stoi(filename.substr(2, filename.find('_', 2)-2));
        
        // Load image (already cropped)
        cv::Mat img = cv::imread(entry.path().string());
        if (img.empty()) continue;
        
        // Resize to standard size (32x32)
        cv::Mat resized;
        cv::resize(img, resized, cv::Size(32, 32));
        
        // Process image
        cv::Mat binary;
        adaptive_binarize(resized, binary);
        
        // Generate 9 shifted versions (±1 pixel tolerance)
        for (int dx : {-1, 0, 1}) {
            for (int dy : {-1, 0, 1}) {
                Template t;
                t.letter = letter;
                t.rotation = rotation;
                
                // Shift and pack
                center_and_pack(binary, t.bits, dx, dy);
                
                // Save to file
                out.write(&t.letter, 1);
                out.write(reinterpret_cast<char*>(&t.rotation), sizeof(int));
                out.write(reinterpret_cast<char*>(t.bits.data()), 128);
            }
        }
    }
    return 0;
}