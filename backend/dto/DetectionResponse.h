#pragma once

#include <string>
#include <vector>
#include "BoundingBox.h"

namespace spectra::vision::dto {

// Application Layer — outbound DTO serialised to JSON by the controller.

struct DetectedObject {
    std::string classLabel;
    int         classId{-1};
    float       confidence{0.0f};
    BoundingBox bbox{};
};

struct DetectionResponse {
    std::string               modelId;
    std::int64_t              inferenceTimeMs{0};
    std::vector<DetectedObject> detections;
};

} // namespace spectra::vision::dto
