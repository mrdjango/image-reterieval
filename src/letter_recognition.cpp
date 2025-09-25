#include "letter_recognition.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <omp.h>
#include <map>
#include <algorithm>

// Platform-specific includes
#if defined(__arm__) || defined(__aarch64__)
    #include <arm_neon.h>
#elif defined(__x86_64__)
    #include <immintrin.h>
    #include <emmintrin.h>
#endif

std::vector<Template> templates;
int SAFE_THRESHOLD = 200;  // Adjusted for 64x64 templates (512 bytes vs 8192 bytes)

// Character-specific confidence adjustment function
double get_confidence_adjustment(char letter, char known_letter) {
    // Base adjustments for common misclassifications
    std::map<char, double> base_adjustments = {
        {'a', -20},  // Give 'a' a significant bonus (lower distance)
        {'o', +15},  // Give 'o' a penalty (higher distance)
        {'u', +10},  // 'u' sometimes confused with 'o'
        {'e', +5},   // 'e' sometimes gets confused
        {'n', +5},   // 'n' penalty
        {'6', -5},
        {'9', +5}
    };
    
    double adjustment = base_adjustments.count(letter) ? base_adjustments[letter] : 0.0;
    
    // Dynamic adjustment based on known letter
    if (known_letter != '?') {
        if (letter == known_letter) {
            adjustment -= 30;  // Strong bonus for known correct letter
        } else {
            // Give penalty to commonly confused letters
            if ((known_letter == 'a' && letter == 'o') || 
                (known_letter == 'o' && letter == 'a')) {
                adjustment += 25;  // Strong penalty for known confusion
            } else if ((known_letter == 'a' && letter == 'u') ||
                      (known_letter == 'u' && letter == 'a')) {
                adjustment += 20;
            } else {
                adjustment += 10;  // General penalty for other letters
            }
        }
    }
    
    return adjustment;
}

// Removed gpu_warp function as coordinates are no longer needed

uint16_t hamming_distance(const uint8_t* a, const uint8_t* b) {
    #if defined(__arm__) || defined(__aarch64__)
    // ARM NEON implementation for 512 bytes
    uint8x16_t sum = vdupq_n_u8(0);
    for(int i=0; i<512; i+=16) {
        uint8x16_t va = vld1q_u8(a + i);
        uint8x16_t vb = vld1q_u8(b + i);
        uint8x16_t vc = veorq_u8(va, vb);
        sum = vaddq_u8(sum, vcntq_u8(vc));
    }
    return vaddlvq_u8(sum);
    #elif defined(__x86_64__) && defined(__SSE4_2__)
    // x86_64 SSE implementation (only if SSE4.2 is available) for 512 bytes
    __m128i sum = _mm_setzero_si128();
    for(int i=0; i<512; i+=16) {
        __m128i va = _mm_loadu_si128((__m128i*)(a + i));
        __m128i vb = _mm_loadu_si128((__m128i*)(b + i));
        __m128i vc = _mm_xor_si128(va, vb);
        
        // Count bits using popcnt (SSE4.2)
        __m128i count = _mm_setzero_si128();
        for(int j=0; j<16; j++) {
            uint8_t byte = _mm_extract_epi8(vc, j);
            count = _mm_insert_epi8(count, __builtin_popcount(byte), j);
        }
        sum = _mm_add_epi8(sum, count);
    }
    
    // Sum up the result
    uint16_t result = 0;
    for(int i=0; i<16; i++) {
        result += _mm_extract_epi8(sum, i);
    }
    return result;
    #else
    // Fallback implementation (works on all platforms) for 512 bytes
    uint16_t result = 0;
    for(int i=0; i<512; i++) {
        uint8_t diff = a[i] ^ b[i];
        result += __builtin_popcount(diff);
    }
    return result;
    #endif
}

void adaptive_binarize(const cv::Mat& src, cv::Mat& dst) {
    cv::Mat gray;
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    
    // Optimized mean calculation for 64x64 (4096 pixels)
    uint32_t sum = 0;
    const uint8_t* p = gray.data;
    for(int i=0; i<4096; i++) sum += p[i];
    uint8_t threshold = sum >> 12;  // Divide by 4096
    
    dst.create(64, 64, CV_8U);
    
    // Count pixels above and below threshold to determine letter polarity
    int above_threshold = 0, below_threshold = 0;
    for(int i=0; i<4096; i++) {
        if(p[i] > threshold) above_threshold++;
        else below_threshold++;
    }
    
    // If more pixels are dark, assume dark letters on light background
    // If more pixels are light, assume light letters on dark background
    bool dark_letters = (below_threshold > above_threshold);
    
    for(int i=0; i<4096; i++) {
        bool is_letter_pixel = dark_letters ? (p[i] <= threshold) : (p[i] > threshold);
        dst.data[i] = is_letter_pixel ? 255 : 0;
    }
}

void center_and_pack(const cv::Mat& bin, std::vector<uint8_t>& packed) {
    // Centroid calculation for 64x64
    int cx = 0, cy = 0, count = 0;
    for(int y=0; y<64; y++) {
        for(int x=0; x<64; x++) {
            if(bin.at<uint8_t>(y,x)) {
                cx += x; cy += y; count++;
            }
        }
    }
    cx = (count > 0) ? cx / count : 32;
    cy = (count > 0) ? cy / count : 32;
    
    // Pack into 512 bytes (4096 bits = 64x64)
    packed.resize(512, 0);
    for(int y=0; y<64; y++) {
        for(int x=0; x<64; x++) {
            int dx = x - cx + 32;
            int dy = y - cy + 32;
            if(dx >= 0 && dx < 64 && dy >= 0 && dy < 64) {
                int bit_pos = dy * 64 + dx;
                int byte_pos = bit_pos / 8;
                int bit_offset = bit_pos % 8;
                if(bin.at<uint8_t>(y,x)) {
                    packed[byte_pos] |= (1 << bit_offset);
                }
            }
        }
    }
}

void load_templates(const std::string& path) {
    templates.clear();
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open templates file: " + path);
    }
    
    std::string line;
    int line_number = 0;
    
    while(std::getline(file, line)) {
        line_number++;
        if(line.empty()) continue;
        
        // Check if line has minimum required length
        if (line.length() < 3) {
            std::cerr << "Warning: Skipping invalid line " << line_number << ": " << line << std::endl;
            continue;
        }
        
        Template t;
        t.letter = line[0];
        
        // Find the comma to separate rotation from binary data
        size_t comma_pos = line.find(',');
        if (comma_pos == std::string::npos) {
            std::cerr << "Warning: Skipping line " << line_number << " (no comma found): " << line << std::endl;
            continue;
        }
        
        // Parse rotation number
        std::string rotation_str = line.substr(2, comma_pos - 2);
        try {
            t.rotation = std::stoi(rotation_str);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Warning: Invalid rotation number '" << rotation_str << "' in line " << line_number << ": " << line << std::endl;
            continue;
        } catch (const std::out_of_range& e) {
            std::cerr << "Warning: Rotation number out of range '" << rotation_str << "' in line " << line_number << ": " << line << std::endl;
            continue;
        }
        
        // Parse binary string
        std::string bits_str = line.substr(comma_pos + 1);
        if (bits_str.length() < 512) {
            std::cerr << "Warning: Binary string too short in line " << line_number << ": " << line << std::endl;
            continue;
        }
        
        t.bits.resize(512);
        for(int i=0; i<512; i++) {
            t.bits[i] = (bits_str[i] == '1') ? 0xFF : 0x00;
        }
        
        templates.push_back(t);
    }
    
    std::cout << "Loaded " << templates.size() << " templates from " << path << std::endl;
}

void load_templates_binary(const std::string& path) {
    templates.clear();
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open binary templates file: " + path);
    }
    
    while (file.good()) {
        Template t;
        
        // Read letter (1 byte)
        if (!file.read(&t.letter, 1)) break;
        
        // Read rotation (4 bytes)
        if (!file.read(reinterpret_cast<char*>(&t.rotation), sizeof(int))) break;
        
        // Read bits (512 bytes for 64x64)
        t.bits.resize(512);
        if (!file.read(reinterpret_cast<char*>(t.bits.data()), 512)) break;
        
        templates.push_back(t);
    }
    
    std::cout << "Loaded " << templates.size() << " templates from " << path << std::endl;
}

char recognize_letter(const cv::Mat& image) {
    // Debug: Print input image info
    std::cout << "Input image: " << image.cols << "x" << image.rows << " channels: " << image.channels() << std::endl;
    
    // Resize image to 64x64 (same as templates)
    cv::Mat resized;
    cv::resize(image, resized, cv::Size(64, 64));
    
    // Debug: Save resized image
    debug_save_image(resized, "debug_resized.jpg");
    
    cv::Mat binary;
    adaptive_binarize(resized, binary);
    
    // Debug: Save binary image
    debug_save_image(binary, "debug_binary.jpg");
    
    std::vector<uint8_t> packed;
    center_and_pack(binary, packed);
    
    // Debug: Check packed data
    std::cout << "Packed data size: " << packed.size() << " bytes" << std::endl;
    int non_zero_bytes = 0;
    for (int i = 0; i < packed.size(); i++) {
        if (packed[i] != 0) non_zero_bytes++;
    }
    std::cout << "Non-zero bytes in packed data: " << non_zero_bytes << std::endl;
    
    char best_match = '?';
    int min_distance = INT_MAX;
    
    // Debug: Check if templates are loaded
    if (templates.empty()) {
        std::cerr << "Warning: No templates loaded!" << std::endl;
        return '?';
    }
    
    // Debug: Print template stats
    debug_print_template_stats();
    
    for(const auto& t : templates) {
        int distance = hamming_distance(packed.data(), t.bits.data());
        if(distance < min_distance) {
            min_distance = distance;
            best_match = t.letter;
        }
    }
    
    // Debug: Print distance information
    std::cout << "Min distance: " << min_distance << " (threshold: " << SAFE_THRESHOLD << ")" << std::endl;
    std::cout << "Best match: " << best_match << std::endl;
    
    // Debug: Print top 5 matches
    std::vector<std::pair<char, int>> distances;
    for(const auto& t : templates) {
        int distance = hamming_distance(packed.data(), t.bits.data());
        distances.push_back({t.letter, distance});
    }
    std::sort(distances.begin(), distances.end(), 
              [](const auto& a, const auto& b) { return a.second < b.second; });
    
    std::cout << "Top 5 matches:" << std::endl;
    for (int i = 0; i < std::min(5, (int)distances.size()); i++) {
        std::cout << "  " << distances[i].first << ": " << distances[i].second << std::endl;
    }
    
    return (min_distance <= SAFE_THRESHOLD) ? best_match : '?';
}

RecognitionResult recognize_letter_with_rotation(const cv::Mat& image, char known_letter) {
    // Debug: Print input image info
    std::cout << "Input image: " << image.cols << "x" << image.rows << " channels: " << image.channels() << std::endl;
    if (known_letter != '?') {
        std::cout << "Known letter hint: " << known_letter << std::endl;
    }
    
    // Resize image to 64x64 (same as templates)
    cv::Mat resized;
    cv::resize(image, resized, cv::Size(64, 64));
    
    // Debug: Save resized image
    debug_save_image(resized, "debug_resized.jpg");
    
    cv::Mat binary;
    adaptive_binarize(resized, binary);
    
    // Debug: Save binary image
    debug_save_image(binary, "debug_binary.jpg");
    
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
    
    // Find the best matching template (including rotation) with dynamic adjustments
    for(const auto& t : templates) {
        int distance = hamming_distance(packed.data(), t.bits.data());
        
        // Apply confidence adjustment based on known letter
        double adjusted_distance = distance + get_confidence_adjustment(t.letter, known_letter);
        
        if(adjusted_distance < best_result.confidence) {
            best_result.letter = t.letter;
            best_result.rotation = t.rotation;
            best_result.confidence = adjusted_distance;
        }
    }
    
    // Debug: Print distance information
    std::cout << "Min adjusted distance: " << best_result.confidence << " (threshold: " << SAFE_THRESHOLD << ")" << std::endl;
    std::cout << "Best match: " << best_result.letter << " (rotation: " << best_result.rotation << "°)" << std::endl;
    
    // Debug: Print top 5 matches with rotation and adjustments
    std::vector<std::pair<RecognitionResult, double>> distances;
    for(const auto& t : templates) {
        int distance = hamming_distance(packed.data(), t.bits.data());
        double adjusted_distance = distance + get_confidence_adjustment(t.letter, known_letter);
        RecognitionResult result(t.letter, t.rotation, adjusted_distance);
        distances.push_back({result, adjusted_distance});
    }
    std::sort(distances.begin(), distances.end(), 
              [](const auto& a, const auto& b) { return a.second < b.second; });
    
    std::cout << "Top 5 matches (with dynamic adjustments):" << std::endl;
    for (int i = 0; i < std::min(5, (int)distances.size()); i++) {
        const auto& result = distances[i].first;
        double original_dist = distances[i].second - get_confidence_adjustment(result.letter, known_letter);
        double adjustment = get_confidence_adjustment(result.letter, known_letter);
        std::cout << "  " << result.letter << " (rotation: " << result.rotation << "°): " 
                  << original_dist << " -> " << distances[i].second 
                  << " (adj: " << adjustment << ")" << std::endl;
    }
    
    // Use dynamic threshold based on known letter
    int dynamic_threshold = SAFE_THRESHOLD;
    if (known_letter != '?') {
        dynamic_threshold += 400;  // Very permissive when we have a hint
        std::cout << "Using dynamic threshold: " << dynamic_threshold << " (hint: " << known_letter << ")" << std::endl;
    } else {
        dynamic_threshold += 300;  // More permissive for general case
        std::cout << "Using adjusted threshold: " << dynamic_threshold << std::endl;
    }
    
    // If confidence is too low, mark as unknown
    if (best_result.confidence > dynamic_threshold) {
        best_result.letter = '?';
        best_result.rotation = 0;
    }
    
    return best_result;
}

void calibrate_threshold(const std::string& validation_dir) {
    // Simple threshold calibration based on validation data
    // This could be enhanced with machine learning
    SAFE_THRESHOLD = 200;  // Updated for 64x64 templates
}

void debug_save_image(const cv::Mat& img, const std::string& filename) {
    try {
        cv::imwrite(filename, img);
        std::cout << "Debug: Saved image to " << filename << std::endl;
    } catch (const cv::Exception& e) {
        std::cerr << "Debug: Failed to save image " << filename << ": " << e.what() << std::endl;
    }
}

void debug_print_template_stats() {
    std::cout << "Template Statistics:" << std::endl;
    std::cout << "  Total templates: " << templates.size() << std::endl;
    
    if (templates.empty()) return;
    
    // Count templates per letter
    std::map<char, int> letter_counts;
    for (const auto& t : templates) {
        letter_counts[t.letter]++;
    }
    
    std::cout << "  Templates per letter:" << std::endl;
    for (const auto& pair : letter_counts) {
        std::cout << "    " << pair.first << ": " << pair.second << std::endl;
    }
    
    // Check template data validity
    int valid_templates = 0;
    for (const auto& t : templates) {
        if (t.bits.size() == 128) valid_templates++;
    }
    std::cout << "  Valid templates (128 bytes): " << valid_templates << "/" << templates.size() << std::endl;
}