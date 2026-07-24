#pragma once

#include "IDetectionStrategy.h"

namespace spectra::vision::strategy {

// Application Layer — decodes YOLO-format outputs.
// Handles anchor-based (v3/v4) and anchor-free (v8) output tensors.

class YoloDetectionStrategy : public IDetectionStrategy {
public:
    YoloDetectionStrategy() = default;

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
