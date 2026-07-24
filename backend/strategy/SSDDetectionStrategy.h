#pragma once

#include "IDetectionStrategy.h"

namespace spectra::vision::strategy {

// Application Layer — decodes SSD / MobileNet-SSD output tensors.
// Handles the [1, N, 7] detection format (class, score, x1, y1, x2, y2, ...).

class SSDDetectionStrategy : public IDetectionStrategy {
public:
    SSDDetectionStrategy() = default;

    std::vector<dto::BoundingBox> postprocess(
        const void*  rawOutput,
        std::size_t  outputSize,
        int          frameWidth,
        int          frameHeight,
        float        confidenceThreshold,
        float        nmsThreshold) const override;

    std::string strategyName() const override;
};

} // namespace spectra::vision::strategy
