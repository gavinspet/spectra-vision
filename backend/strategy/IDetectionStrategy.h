#pragma once

#include <vector>
#include "dto/BoundingBox.h"

namespace spectra::vision::strategy {

// Domain / Application Layer — decouples raw model outputs from bounding-box
// post-processing (confidence filtering, NMS, coordinate scaling).
// Each architecture (YOLO, SSD, ...) implements its own decoding logic.

class IDetectionStrategy {
public:
    virtual ~IDetectionStrategy() = default;

    virtual std::vector<dto::BoundingBox> postprocess(
        const void*  rawOutput,
        std::size_t  outputSize,
        int          frameWidth,
        int          frameHeight,
        float        confidenceThreshold,
        float        nmsThreshold) const = 0;

    virtual std::string strategyName() const = 0;
};

} // namespace spectra::vision::strategy
