#pragma once
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

// Raspberry Pi hardware acceleration
#define USE_OPENGL_ES

struct Template {
    char letter;
    int rotation;
    std::vector<uint8_t> bits;
};

extern std::vector<Template> templates;
extern int SAFE_THRESHOLD;

void load_templates(const std::string& path);
char recognize_letter(const cv::Mat& image, const std::vector<cv::Point2f>& cube_coords);
void calibrate_threshold(const std::string& validation_dir);