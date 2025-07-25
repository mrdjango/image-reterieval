#include "letter_recognition.h"
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <image_path> [templates_path]" << std::endl;
        std::cout << "  image_path: Path to the image containing letters to recognize" << std::endl;
        std::cout << "  templates_path: Path to templates file (default: ../templates/templates.txt)" << std::endl;
        return 1;
    }

    std::string image_path = argv[1];
    std::string templates_path = (argc > 2) ? argv[2] : "../templates/templates.txt";

    // Load image
    cv::Mat image = cv::imread(image_path);
    if (image.empty()) {
        std::cerr << "Error: Could not load image " << image_path << std::endl;
        return 1;
    }

    // Load templates
    try {
        load_templates(templates_path);
        std::cout << "Loaded " << templates.size() << " templates from " << templates_path << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error loading templates: " << e.what() << std::endl;
        return 1;
    }

    // Recognize letter (no coordinates needed for cropped images)
    char recognized = recognize_letter(image);
    
    std::cout << "Recognized letter: " << recognized << std::endl;

    // Display result
    cv::putText(image, std::string("Letter: ") + recognized, 
                cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);

    cv::imshow("Letter Recognition Result", image);
    cv::waitKey(0);

    return 0;
}
