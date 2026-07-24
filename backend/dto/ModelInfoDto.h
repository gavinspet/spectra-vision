#pragma once

#include <string>

namespace spectra::vision::dto {

// Application Layer — outbound summary of a loaded model, returned by ModelController.

struct ModelInfoDto {
    std::string modelId;
    std::string modelType;
    std::string detectionStrategy;
    bool        loaded{false};
    int         inputWidth{0};
    int         inputHeight{0};
};

} // namespace spectra::vision::dto
