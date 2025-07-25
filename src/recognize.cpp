#include "letter_recognition.h"
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>

int main(int argc, char** argv) {
    // Load templates
    load_templates("templates.bin");
    
    // Load test coordinates
    std::ifstream coord_file("test_coordinates.csv");
    std::string line;
    
    while (std::getline(coord_file, line)) {
        std::istringstream ss(line);
        std::string image_name;
        float coords[8];
        
        // Parse CSV line
        std::getline(ss, image_name, ',');
        for (int i = 0; i < 8; i++) {
            std::string val;
            std::getline(ss, val, ',');
            coords[i] = std::stof(val);
        }
        
        // Prepare points
        std::vector<cv::Point2f> cube_coords = {
            {coords[0], coords[1]},
            {coords[2], coords[3]},
            {coords[4], coords[5]},
            {coords[6], coords[7]}
        };
        
        // Load test image
        cv::Mat image = cv::imread("test_images/" + image_name);
        if (image.empty()) continue;
        
        // Recognize letter
        char letter = recognize_letter(image, cube_coords);
        
        std::cout << "Image: " << image_name 
                  << " | Detected letter: " << letter << "\n";
    }
    return 0;
}