#include "letter_recognition.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

// Enhanced binarization function
void enhanced_binarize(const cv::Mat& src, cv::Mat& dst) {
    cv::Mat gray;
    if (src.channels() == 3) {
        cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = src.clone();
    }
    
    // Apply Gaussian blur to reduce noise
    cv::GaussianBlur(gray, gray, cv::Size(3, 3), 0);
    
    // Use Otsu's method for better threshold
    cv::threshold(gray, dst, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
    
    // Apply morphological operations to preserve character structure
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
    cv::morphologyEx(dst, dst, cv::MORPH_CLOSE, kernel);
}

// Character-specific confidence adjustment
double get_confidence_adjustment(char letter) {
    // Characters that are often confused get bonus/penalty
    std::map<char, double> adjustments = {
        {'a', -15},  // Give 'a' a bonus (lower distance)
        {'o', +10},  // Give 'o' a penalty (higher distance)
        {'e', +5},   // 'e' sometimes gets confused
        {'6', -5},
        {'9', +5}
    };
    
    return adjustments.count(letter) ? adjustments[letter] : 0.0;
}

// Enhanced recognition with improved processing
RecognitionResult recognize_letter_enhanced(const cv::Mat& image) {
    // Debug: Print input image info
    std::cout << "Input image: " << image.cols << "x" << image.rows << " channels: " << image.channels() << std::endl;
    
    // Resize image to 64x64 (same as templates)
    cv::Mat resized;
    cv::resize(image, resized, cv::Size(64, 64));
    
    // Debug: Save resized image
    debug_save_image(resized, "debug_resized_enhanced.jpg");
    
    cv::Mat binary;
    enhanced_binarize(resized, binary);  // Use enhanced binarization
    
    // Debug: Save binary image
    debug_save_image(binary, "debug_binary_enhanced.jpg");
    
    std::vector<uint8_t> packed;
    center_and_pack(binary, packed);
    
    // Debug: Check packed data
    std::cout << "Packed data size: " << packed.size() << " bytes" << std::endl;
    int non_zero_bytes = 0;
    for (int i = 0; i < packed.size(); i++) {
        if (packed[i] != 0) non_zero_bytes++;
    }
    std::cout << "Non-zero bytes in packed data: " << non_zero_bytes << std::endl;
    
    RecognitionResult best_result;
    
    // Debug: Check if templates are loaded
    if (templates.empty()) {
        std::cerr << "Warning: No templates loaded!" << std::endl;
        return best_result;
    }
    
    // Debug: Print template stats
    debug_print_template_stats();
    
    for(const auto& t : templates) {
        int distance = hamming_distance(packed.data(), t.bits.data());
        
        // Apply confidence adjustment
        double adjusted_distance = distance + get_confidence_adjustment(t.letter);
        
        if(adjusted_distance < best_result.confidence) {
            best_result.letter = t.letter;
            best_result.rotation = t.rotation;
            best_result.confidence = adjusted_distance;
        }
    }
    
    // Debug: Print distance information
    std::cout << "Min distance: " << best_result.confidence << " (threshold: " << SAFE_THRESHOLD << ")" << std::endl;
    std::cout << "Best match: " << best_result.letter << " (rotation: " << best_result.rotation << "°)" << std::endl;
    
    // Debug: Print top 5 matches with rotation and adjustments
    std::vector<std::pair<RecognitionResult, double>> distances;
    for(const auto& t : templates) {
        int distance = hamming_distance(packed.data(), t.bits.data());
        double adjusted_distance = distance + get_confidence_adjustment(t.letter);
        RecognitionResult result(t.letter, t.rotation, adjusted_distance);
        distances.push_back({result, adjusted_distance});
    }
    std::sort(distances.begin(), distances.end(), 
              [](const auto& a, const auto& b) { return a.second < b.second; });
    
    std::cout << "Top 5 matches (with adjustments):" << std::endl;
    for (int i = 0; i < std::min(5, (int)distances.size()); i++) {
        const auto& result = distances[i].first;
        double original_dist = distances[i].second - get_confidence_adjustment(result.letter);
        std::cout << "  " << result.letter << " (rotation: " << result.rotation << "°): " 
                  << original_dist << " -> " << distances[i].second 
                  << " (adj: " << get_confidence_adjustment(result.letter) << ")" << std::endl;
    }
    
    // Use enhanced threshold - more lenient for adjusted scores
    int enhanced_threshold = SAFE_THRESHOLD + 100;  // More forgiving threshold
    
    // If confidence is too low, mark as unknown
    if (best_result.confidence > enhanced_threshold) {
        best_result.letter = '?';
        best_result.rotation = 0;
    }
    
    std::cout << "Enhanced threshold: " << enhanced_threshold << std::endl;
    
    return best_result;
}

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
    std::cout << "COMPARISON SUMMARY" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    if (original_result.letter != enhanced_result.letter) {
        std::cout << "🔄 CHANGE: " << original_result.letter << " -> " << enhanced_result.letter << std::endl;
        if (enhanced_result.letter == 'a' && (original_result.letter == 'o' || original_result.letter == '?')) {
            std::cout << "✅ IMPROVEMENT: Detected 'a' where original failed or misclassified" << std::endl;
        } else if (enhanced_result.letter == 'o' && original_result.letter == 'a') {
            std::cout << "⚠️  REGRESSION: Changed 'a' to 'o' (verify if correct)" << std::endl;
        } else if (enhanced_result.letter != '?' && original_result.letter == '?') {
            std::cout << "✅ IMPROVEMENT: Detected '" << enhanced_result.letter << "' where original failed" << std::endl;
        }
    } else {
        std::cout << "✓ SAME: Both methods detected '" << original_result.letter << "'" << std::endl;
    }
    
    double confidence_improvement = original_result.confidence - enhanced_result.confidence;
    std::cout << "Confidence improvement: " << confidence_improvement << std::endl;
    
    if (confidence_improvement > 0) {
        std::cout << "✅ Enhanced method has better confidence" << std::endl;
    } else if (confidence_improvement < 0) {
        std::cout << "⚠️  Enhanced method has worse confidence" << std::endl;
    } else {
        std::cout << "= Same confidence" << std::endl;
    }
    
    return 0;
}