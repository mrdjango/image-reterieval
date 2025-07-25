#include "letter_recognition.h"
#include <arm_neon.h>
#include <fstream>
#include <omp.h>

std::vector<Template> templates;
int SAFE_THRESHOLD = 15;  // Default, calibrated during init

#ifdef USE_OPENGL_ES
#include <EGL/egl.h>
#include <GLES3/gl3.h>

GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    return shader;
}
#endif

void gpu_warp(const cv::Mat& src, std::vector<cv::Point2f> src_pts, cv::Mat& dst) {
    #ifdef USE_OPENGL_ES
    static GLuint program = 0;
    static GLuint fbo = 0;
    
    if(program == 0) {
        const char* vshader = R"(
            #version 300 es
            in vec2 position;
            in vec2 texCoord;
            out vec2 vTexCoord;
            void main() {
                gl_Position = vec4(position, 0.0, 1.0);
                vTexCoord = texCoord;
            }
        )";
        
        const char* fshader = R"(
            #version 300 es
            precision mediump float;
            in vec2 vTexCoord;
            uniform sampler2D tex;
            out vec4 fragColor;
            void main() {
                fragColor = texture(tex, vTexCoord);
            }
        )";
        
        GLuint vs = compile_shader(GL_VERTEX_SHADER, vshader);
        GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fshader);
        program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
        glDeleteShader(vs);
        glDeleteShader(fs);
        
        glGenFramebuffers(1, &fbo);
    }
    
    // Create texture
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, src.cols, src.rows, 
                 0, GL_BGR, GL_UNSIGNED_BYTE, src.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    // Setup FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                           GL_TEXTURE_2D, tex, 0);
    
    // Render
    glUseProgram(program);
    glViewport(0, 0, 32, 32);
    
    // Draw quad
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    // Read back
    dst.create(32, 32, CV_8UC3);
    glReadPixels(0, 0, 32, 32, GL_BGR, GL_UNSIGNED_BYTE, dst.data);
    
    glDeleteTextures(1, &tex);
    #else
    std::vector<cv::Point2f> dst_pts = {
        {0,0}, {32,0}, {32,32}, {0,32}
    };
    cv::Mat M = cv::getPerspectiveTransform(src_pts, dst_pts);
    cv::warpPerspective(src, dst, M, {32,32});
    #endif
}

uint16_t neon_hamming(const uint8_t* a, const uint8_t* b) {
    uint8x16_t sum = vdupq_n_u8(0);
    for(int i=0; i<128; i+=16) {
        uint8x16_t va = vld1q_u8(a + i);
        uint8x16_t vb = vld1q_u8(b + i);
        uint8x16_t vc = veorq_u8(va, vb);
        sum = vaddq_u8(sum, vcntq_u8(vc));
    }
    return vaddlvq_u8(sum);
}

void adaptive_binarize(const cv::Mat& src, cv::Mat& dst) {
    cv::Mat gray;
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    
    // ARM-optimized mean calculation
    uint32_t sum = 0;
    const uint8_t* p = gray.data;
    for(int i=0; i<1024; i++) sum += p[i];
    uint8_t threshold = sum >> 10;  // Divide by 1024
    
    dst.create(32, 32, CV_8U);
    for(int i=0; i<1024; i++)
        dst.data[i] = (p[i] > threshold) ? 255 : 0;
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