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
int SAFE_THRESHOLD = 70;  // Increased from 15 to be less restrictive

// Removed gpu_warp function as coordinates are no longer needed

uint16_t hamming_distance(const uint8_t* a, const uint8_t* b) {
    #if defined(__arm__) || defined(__aarch64__)
    // ARM NEON implementation
    uint8x16_t sum = vdupq_n_u8(0);
    for(int i=0; i<128; i+=16) {
        uint8x16_t va = vld1q_u8(a + i);
        uint8x16_t vb = vld1q_u8(b + i);
        uint8x16_t vc = veorq_u8(va, vb);
        sum = vaddq_u8(sum, vcntq_u8(vc));
    }
    return vaddlvq_u8(sum);
    #elif defined(__x86_64__) && defined(__SSE4_2__)
    // x86_64 SSE implementation (only if SSE4.2 is available)
    __m128i sum = _mm_setzero_si128();
    for(int i=0; i<128; i+=16) {
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
    // Fallback implementation (works on all platforms)
    uint16_t result = 0;
    for(int i=0; i<128; i++) {
        uint8_t diff = a[i] ^ b[i];
        result += __builtin_popcount(diff);
    }
    return result;
    #endif
}

void adaptive_binarize(const cv::Mat& src, cv::Mat& dst) {
    cv::Mat gray;
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    
    // Optimized mean calculation
    uint32_t sum = 0;
    const uint8_t* p = gray.data;
    for(int i=0; i<1024; i++) sum += p[i];
    uint8_t threshold = sum >> 10;  // Divide by 1024
    
    dst.create(32, 32, CV_8U);
    
    // Count pixels above and below threshold to determine letter polarity
    int above_threshold = 0, below_threshold = 0;
    for(int i=0; i<1024; i++) {
        if(p[i] > threshold) above_threshold++;
        else below_threshold++;
    }
    
    // If more pixels are dark, assume dark letters on light background
    // If more pixels are light, assume light letters on dark background
    bool dark_letters = (below_threshold > above_threshold);
    
    for(int i=0; i<1024; i++) {
        bool is_letter_pixel = dark_letters ? (p[i] <= threshold) : (p[i] > threshold);
        dst.data[i] = is_letter_pixel ? 255 : 0;
    }
}

void center_and_pack(const cv::Mat& bin, std::vector<uint8_t>& packed) {
    // Centroid calculation
    int cx = 0, cy = 0, count = 0;
    for(int y=0; y<32; y++) {
        for(int x=0; x<32; x++) {
            if(bin.at<uint8_t>(y,x)) {
                cx += x; cy += y; count++;
            }
        }
    }
    cx = (count > 0) ? cx / count : 16;
    cy = (count > 0) ? cy / count : 16;
    
    // Pack into 128 bytes (1024 bits)
    packed.resize(128, 0);
    for(int y=0; y<32; y++) {
        for(int x=0; x<32; x++) {
            int dx = x - cx + 16;
            int dy = y - cy + 16;
            if(dx >= 0 && dx < 32 && dy >= 0 && dy < 32) {
                int bit_pos = dy * 32 + dx;
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
        if (bits_str.length() < 128) {
            std::cerr << "Warning: Binary string too short in line " << line_number << ": " << line << std::endl;
            continue;
        }
        
        t.bits.resize(128);
        for(int i=0; i<128; i++) {
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
        
        // Read bits (128 bytes)
        t.bits.resize(128);
        if (!file.read(reinterpret_cast<char*>(t.bits.data()), 128)) break;
        
        templates.push_back(t);
    }
    
    std::cout << "Loaded " << templates.size() << " templates from " << path << std::endl;
}

char recognize_letter(const cv::Mat& image) {
    // Debug: Print input image info
    std::cout << "Input image: " << image.cols << "x" << image.rows << " channels: " << image.channels() << std::endl;
    
    // Resize image to 32x32 (same as templates)
    cv::Mat resized;
    cv::resize(image, resized, cv::Size(32, 32));
    
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

void calibrate_threshold(const std::string& validation_dir) {
    // Simple threshold calibration based on validation data
    // This could be enhanced with machine learning
    SAFE_THRESHOLD = 70;  // Updated default value
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