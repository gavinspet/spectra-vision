#include "SSDDetectionStrategy.h"

namespace spectra::vision::strategy {

std::vector<dto::BoundingBox> SSDDetectionStrategy::postprocess(
    const void* /*rawOutput*/,
    std::size_t /*outputSize*/,
    int         /*frameWidth*/,
    int         /*frameHeight*/,
    float       /*confidenceThreshold*/,
    float       /*nmsThreshold*/) const
{
    // TODO: iterate detection tensor rows
    // TODO: filter rows where score > confidenceThreshold
    // TODO: denormalise box coordinates to pixel space
    return {};
}

std::string SSDDetectionStrategy::strategyName() const { return "ssd"; }

} // namespace spectra::vision::strategy
