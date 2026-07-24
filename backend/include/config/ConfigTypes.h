#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace spectra::vision::config {

// ═════════════════════════════════════════════════════════════════════════════
// VALUE TYPES — Immutable configuration structs (copiable, no mutable state)
// ═════════════════════════════════════════════════════════════════════════════

/// HTTP server configuration
struct ServerConfig {
    std::string host{"0.0.0.0"};
    std::uint16_t port{8080};
    int threadCount{16};
    std::string logPath{"./logs"};
    std::string logLevel{"info"};
};

/// Security and authentication configuration
struct SecurityConfig {
    bool enableAuth{true};
    int rateLimitRps{100};
    std::string jwtSecret;
};

/// Input preprocessing configuration
struct PreprocessingConfig {
    int targetWidth{224};
    int targetHeight{224};
    float normalizeScale{1.0f / 255.0f};
    float meanR{0.0f};
    float meanG{0.0f};
    float meanB{0.0f};
};

/// Input shape for model inference
struct InputShape {
    int batch{1};
    int channels{3};
    int height{640};
    int width{640};
};

/// Single model configuration
struct ModelConfig {
    std::string modelId;
    std::string modelPath;
    std::string modelType;       // "onnx", "opencv-dnn"
    std::string detectionStrategy;  // "emotion-classification", "yolo", "ssd"
    std::vector<std::string> classLabels;
    InputShape inputShape;
    float confidenceThreshold{0.5f};
    float nmsThreshold{0.45f};

    // Equality comparison for testing
    bool operator==(const ModelConfig& other) const;
};

}  // namespace spectra::vision::config
