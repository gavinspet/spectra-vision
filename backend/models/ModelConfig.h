#pragma once

#include <string>
#include <vector>

namespace spectra::vision::models {

// Domain Layer — describes a registered model artefact and its runtime parameters.
// Loaded from config/ModelRegistryConfig and used by ModelService.

struct InputShape {
    int batch{1};
    int channels{3};
    int height{640};
    int width{640};
};

struct ModelConfig {
    std::string              modelId;
    std::string              modelPath;
    std::string              modelType;       // "onnx" | "opencv-dnn"
    std::string              detectionStrategy; // "yolo" | "ssd"
    std::vector<std::string> classLabels;
    InputShape               inputShape{};
    float                    confidenceThreshold{0.5f};
    float                    nmsThreshold{0.45f};
};

} // namespace spectra::vision::models
