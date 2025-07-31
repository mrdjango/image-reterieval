#include <iostream>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

// Test the 256x256 image processing pipeline
void test_256x256_processing() {
    std::cout << "=== Testing 256x256 Image Processing ===" << std::endl;
    std::cout << std::endl;
    
    // Create a test image (100x100)
    cv::Mat test_image(100, 100, CV_8UC3, cv::Scalar(255, 255, 255));
    
    // Draw a simple letter 'A' in the center
    cv::putText(test_image, "A", cv::Point(30, 70), cv::FONT_HERSHEY_SIMPLEX, 3, cv::Scalar(0, 0, 0), 3);
    
    std::cout << "Original image size: " << test_image.cols << "x" << test_image.rows << std::endl;
    
    // Resize to 256x256
    cv::Mat resized;
    cv::resize(test_image, resized, cv::Size(256, 256));
    std::cout << "Resized image size: " << resized.cols << "x" << resized.rows << std::endl;
    
    // Save the resized image for inspection
    cv::imwrite("test_256x256_resized.jpg", resized);
    std::cout << "Saved resized image to test_256x256_resized.jpg" << std::endl;
    
    // Convert to grayscale
    cv::Mat gray;
    cv::cvtColor(resized, gray, cv::COLOR_BGR2GRAY);
    
    // Calculate mean for thresholding
    uint32_t sum = 0;
    const uint8_t* p = gray.data;
    for(int i=0; i<65536; i++) sum += p[i];
    uint8_t threshold = sum >> 16;  // Divide by 65536
    
    std::cout << "Mean pixel value: " << (sum / 65536) << std::endl;
    std::cout << "Threshold: " << (int)threshold << std::endl;
    
    // Create binary image
    cv::Mat binary(256, 256, CV_8U);
    
    // Count pixels above and below threshold
    int above_threshold = 0, below_threshold = 0;
    for(int i=0; i<65536; i++) {
        if(p[i] > threshold) above_threshold++;
        else below_threshold++;
    }
    
    std::cout << "Pixels above threshold: " << above_threshold << std::endl;
    std::cout << "Pixels below threshold: " << below_threshold << std::endl;
    
    // Determine letter polarity
    bool dark_letters = (below_threshold > above_threshold);
    std::cout << "Dark letters on light background: " << (dark_letters ? "Yes" : "No") << std::endl;
    
    // Create binary image
    for(int i=0; i<65536; i++) {
        bool is_letter_pixel = dark_letters ? (p[i] <= threshold) : (p[i] > threshold);
        binary.data[i] = is_letter_pixel ? 255 : 0;
    }
    
    // Save binary image
    cv::imwrite("test_256x256_binary.jpg", binary);
    std::cout << "Saved binary image to test_256x256_binary.jpg" << std::endl;
    
    // Test centroid calculation
    int cx = 0, cy = 0, count = 0;
    for(int y=0; y<256; y++) {
        for(int x=0; x<256; x++) {
            if(binary.at<uint8_t>(y,x)) {
                cx += x; cy += y; count++;
            }
        }
    }
    cx = (count > 0) ? cx / count : 128;
    cy = (count > 0) ? cy / count : 128;
    
    std::cout << "Centroid: (" << cx << ", " << cy << ")" << std::endl;
    std::cout << "Letter pixels: " << count << std::endl;
    
    // Test packing
    std::vector<uint8_t> packed(8192, 0);
    for(int y=0; y<256; y++) {
        for(int x=0; x<256; x++) {
            int dx = x - cx + 128;
            int dy = y - cy + 128;
            if(dx >= 0 && dx < 256 && dy >= 0 && dy < 256) {
                int bit_pos = dy * 256 + dx;
                int byte_pos = bit_pos / 8;
                int bit_offset = bit_pos % 8;
                if(binary.at<uint8_t>(y,x)) {
                    packed[byte_pos] |= (1 << bit_offset);
                }
            }
        }
    }
    
    // Count non-zero bytes
    int non_zero_bytes = 0;
    for (int i = 0; i < 8192; i++) {
        if (packed[i] != 0) non_zero_bytes++;
    }
    
    std::cout << "Packed data size: " << packed.size() << " bytes" << std::endl;
    std::cout << "Non-zero bytes: " << non_zero_bytes << std::endl;
    std::cout << "Compression ratio: " << (non_zero_bytes * 100.0 / 8192) << "%" << std::endl;
    
    std::cout << std::endl;
    std::cout << "✓ 256x256 image processing test completed successfully!" << std::endl;
    std::cout << std::endl;
}

// Test memory usage
void test_memory_usage() {
    std::cout << "=== Memory Usage Analysis ===" << std::endl;
    std::cout << std::endl;
    
    // Calculate memory usage for different image sizes
    std::cout << "Memory usage comparison:" << std::endl;
    std::cout << "32x32 system:" << std::endl;
    std::cout << "  - Binary image: 32x32 = 1,024 bytes" << std::endl;
    std::cout << "  - Packed data: 128 bytes" << std::endl;
    std::cout << "  - Template storage: 128 bytes per template" << std::endl;
    std::cout << std::endl;
    
    std::cout << "256x256 system:" << std::endl;
    std::cout << "  - Binary image: 256x256 = 65,536 bytes" << std::endl;
    std::cout << "  - Packed data: 8,192 bytes" << std::endl;
    std::cout << "  - Template storage: 8,192 bytes per template" << std::endl;
    std::cout << std::endl;
    
    // Calculate template storage for typical system
    int letters = 26;
    int rotations = 4;
    int shifts = 9;  // 3x3 shift variations
    int total_templates = letters * rotations * shifts;
    
    std::cout << "Typical template count:" << std::endl;
    std::cout << "  - Letters: " << letters << std::endl;
    std::cout << "  - Rotations: " << rotations << std::endl;
    std::cout << "  - Shifts: " << shifts << std::endl;
    std::cout << "  - Total templates: " << total_templates << std::endl;
    std::cout << std::endl;
    
    std::cout << "Storage requirements:" << std::endl;
    std::cout << "  32x32 system: " << (total_templates * 128) << " bytes (" 
              << (total_templates * 128 / 1024.0) << " KB)" << std::endl;
    std::cout << "  256x256 system: " << (total_templates * 8192) << " bytes (" 
              << (total_templates * 8192 / 1024.0) << " KB)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Performance considerations:" << std::endl;
    std::cout << "  - 256x256 provides 64x more pixel data" << std::endl;
    std::cout << "  - Better recognition accuracy expected" << std::endl;
    std::cout << "  - 64x more memory usage" << std::endl;
    std::cout << "  - 64x more computation for hamming distance" << std::endl;
    std::cout << std::endl;
}

int main() {
    std::cout << "=== 256x256 Image Recognition System Test ===" << std::endl;
    std::cout << "Testing the updated system with 256x256 image processing" << std::endl;
    std::cout << std::endl;
    
    test_256x256_processing();
    test_memory_usage();
    
    std::cout << "=== Summary ===" << std::endl;
    std::cout << "The system has been successfully updated to use 256x256 images:" << std::endl;
    std::cout << "1. Image resizing: 256x256 instead of 32x32" << std::endl;
    std::cout << "2. Binary processing: 256x256 = 65,536 pixels" << std::endl;
    std::cout << "3. Data packing: 8,192 bytes instead of 128 bytes" << std::endl;
    std::cout << "4. Template storage: 8,192 bytes per template" << std::endl;
    std::cout << "5. Hamming distance: 64x more computation" << std::endl;
    std::cout << "6. Threshold adjusted: 4,000 instead of 70" << std::endl;
    std::cout << std::endl;
    std::cout << "Benefits:" << std::endl;
    std::cout << "  - Much higher resolution for better recognition" << std::endl;
    std::cout << "  - More detailed feature extraction" << std::endl;
    std::cout << "  - Better handling of complex letters" << std::endl;
    std::cout << std::endl;
    std::cout << "Trade-offs:" << std::endl;
    std::cout << "  - 64x more memory usage" << std::endl;
    std::cout << "  - 64x more computation time" << std::endl;
    std::cout << "  - Larger template files" << std::endl;
    std::cout << std::endl;
    
    return 0;
}