#include <iostream>
#include <vector>
#include <string>

// Simplified version of the recognition system for demonstration
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

// Simulated template data for demonstration
std::vector<Template> create_demo_templates() {
    std::vector<Template> templates;
    
    // Create templates for letter 'A' at different rotations
    for (int rotation = 0; rotation < 360; rotation += 90) {
        Template t;
        t.letter = 'A';
        t.rotation = rotation;
        t.bits.resize(128, 0);
        // Simulate different bit patterns for different rotations
        for (int i = 0; i < 128; i++) {
            t.bits[i] = (i + rotation / 90) % 256;
        }
        templates.push_back(t);
    }
    
    // Create templates for letter 'B' at different rotations
    for (int rotation = 0; rotation < 360; rotation += 90) {
        Template t;
        t.letter = 'B';
        t.rotation = rotation;
        t.bits.resize(128, 0);
        // Simulate different bit patterns for different rotations
        for (int i = 0; i < 128; i++) {
            t.bits[i] = (i * 2 + rotation / 90) % 256;
        }
        templates.push_back(t);
    }
    
    return templates;
}

// Simulated hamming distance calculation
uint16_t hamming_distance(const uint8_t* a, const uint8_t* b) {
    uint16_t result = 0;
    for(int i = 0; i < 128; i++) {
        uint8_t diff = a[i] ^ b[i];
        result += __builtin_popcount(diff);
    }
    return result;
}

// Simulated image processing (creates a test pattern)
std::vector<uint8_t> process_test_image() {
    std::vector<uint8_t> packed(128, 0);
    // Create a test pattern that might match 'A' at 90 degrees
    for (int i = 0; i < 128; i++) {
        packed[i] = (i + 1) % 256;  // Similar to A at 90 degrees
    }
    return packed;
}

RecognitionResult recognize_letter_with_rotation(const std::vector<Template>& templates, 
                                               const std::vector<uint8_t>& packed) {
    RecognitionResult best_result;
    
    // Find the best matching template (including rotation)
    for(const auto& t : templates) {
        int distance = hamming_distance(packed.data(), t.bits.data());
        if(distance < best_result.confidence) {
            best_result.letter = t.letter;
            best_result.rotation = t.rotation;
            best_result.confidence = distance;
        }
    }
    
    // If confidence is too low, mark as unknown
    if (best_result.confidence > 100) {  // Threshold
        best_result.letter = '?';
        best_result.rotation = 0;
    }
    
    return best_result;
}

int main() {
    std::cout << "=== Letter Recognition with Rotation Demo ===" << std::endl;
    std::cout << "This demo shows how the system recognizes both letters and their rotation." << std::endl;
    std::cout << std::endl;
    
    // Create demo templates
    std::vector<Template> templates = create_demo_templates();
    std::cout << "Created " << templates.size() << " templates:" << std::endl;
    for (const auto& t : templates) {
        std::cout << "  Letter: " << t.letter << ", Rotation: " << t.rotation << "°" << std::endl;
    }
    std::cout << std::endl;
    
    // Process a test image
    std::vector<uint8_t> test_image = process_test_image();
    std::cout << "Processing test image..." << std::endl;
    
    // Recognize letter with rotation
    RecognitionResult result = recognize_letter_with_rotation(templates, test_image);
    
    std::cout << "\n=== Recognition Results ===" << std::endl;
    std::cout << "Detected letter: " << result.letter << std::endl;
    std::cout << "Detected rotation: " << result.rotation << "°" << std::endl;
    std::cout << "Confidence (hamming distance): " << result.confidence << std::endl;
    
    if (result.letter != '?') {
        std::cout << "✓ Letter recognized successfully!" << std::endl;
        std::cout << "  The system detected that the letter '" << result.letter 
                  << "' is rotated " << result.rotation << " degrees." << std::endl;
    } else {
        std::cout << "✗ Letter not recognized (confidence too low)" << std::endl;
    }
    
    std::cout << "\n=== How it works ===" << std::endl;
    std::cout << "1. The system loads templates for each letter at different rotations" << std::endl;
    std::cout << "2. When processing an image, it compares against ALL templates" << std::endl;
    std::cout << "3. The best match (lowest hamming distance) determines both letter and rotation" << std::endl;
    std::cout << "4. The system returns a RecognitionResult with letter, rotation, and confidence" << std::endl;
    
    std::cout << "\n=== Key Features ===" << std::endl;
    std::cout << "• Rotation-aware recognition (0°, 90°, 180°, 270°)" << std::endl;
    std::cout << "• Confidence scoring based on hamming distance" << std::endl;
    std::cout << "• Backward compatibility with original recognize_letter function" << std::endl;
    std::cout << "• Detailed debugging output for analysis" << std::endl;
    
    return 0;
}