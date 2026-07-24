#pragma once

#include <string>
#include <vector>

namespace spectra::vision::dto {

// Application Layer — inbound DTO from the HTTP presentation layer.
// Validated in the controller before being forwarded to DetectionService.

struct DetectionRequest {
    std::string modelId;
    std::string imageBase64;   // base-64 encoded raw image bytes
    float       confidenceThreshold{0.5f};
    int         maxResults{100};
};

} // namespace spectra::vision::dto
