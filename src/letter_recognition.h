#pragma once
#include <vector>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

// Hardware acceleration - conditional based on platform
#if defined(__arm__) || defined(__aarch64__)
    #define USE_OPENGL_ES
#elif defined(__x86_64__)
    #define USE_OPENGL
#endif

struct Template {
    char letter;
    int rotation;
    std::vector<uint8_t> bits;
};

struct RecognitionResult {
    char letter;
    int rotation;
    int confidence;  // Lower is better (hamming distance)
    
    RecognitionResult() : letter('?'), rotation(0), confidence(INT_MAX) {}
    RecognitionResult(char l, int r, int c) : letter(l), rotation(r), confidence(c) {}
};

extern std::vector<Template> templates;
extern int SAFE_THRESHOLD;

// Core functions
void load_templates(const std::string& path);
void load_templates_binary(const std::string& path);
char recognize_letter(const cv::Mat& image);  // Legacy function
RecognitionResult recognize_letter_with_rotation(const cv::Mat& image);  // New function with rotation
void calibrate_threshold(const std::string& validation_dir);

// Image processing functions
void adaptive_binarize(const cv::Mat& src, cv::Mat& dst);
void center_and_pack(const cv::Mat& bin, std::vector<uint8_t>& packed);
uint16_t hamming_distance(const uint8_t* a, const uint8_t* b);

// Debug functions
void debug_save_image(const cv::Mat& img, const std::string& filename);
void debug_print_template_stats();

// Note: Removed gpu_warp function as coordinates are no longer needed