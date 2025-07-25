#include <iostream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

int main() {
    std::cout << "Current working directory: " << fs::current_path() << std::endl;
    
    // Test dataset path
    std::string dataset_path = "../../dataset";
    std::cout << "Testing dataset path: " << dataset_path << std::endl;
    if (fs::exists(dataset_path)) {
        std::cout << "✓ Dataset directory exists" << std::endl;
        int count = 0;
        for (const auto& entry : fs::directory_iterator(dataset_path)) {
            if (entry.path().extension() == ".jpg") {
                count++;
            }
        }
        std::cout << "Found " << count << " .jpg files in dataset" << std::endl;
    } else {
        std::cout << "✗ Dataset directory does not exist" << std::endl;
    }
    
    // Test test_coordinates.csv path
    std::string coords_path = "../test_coordinates.csv";
    std::cout << "Testing coordinates path: " << coords_path << std::endl;
    if (fs::exists(coords_path)) {
        std::cout << "✓ test_coordinates.csv exists" << std::endl;
    } else {
        std::cout << "✗ test_coordinates.csv does not exist" << std::endl;
    }
    
    // Test test_images path
    std::string test_images_path = "../../test_images";
    std::cout << "Testing test_images path: " << test_images_path << std::endl;
    if (fs::exists(test_images_path)) {
        std::cout << "✓ test_images directory exists" << std::endl;
        int count = 0;
        for (const auto& entry : fs::directory_iterator(test_images_path)) {
            if (entry.path().extension() == ".jpg") {
                count++;
                std::cout << "  Found: " << entry.path().filename() << std::endl;
            }
        }
        std::cout << "Found " << count << " .jpg files in test_images" << std::endl;
    } else {
        std::cout << "✗ test_images directory does not exist" << std::endl;
    }
    
    return 0;
} 