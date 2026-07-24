#include "YoloDetectionStrategy.h"

namespace spectra::vision::strategy {

std::vector<dto::BoundingBox> YoloDetectionStrategy::postprocess(
    const void* /*rawOutput*/,
    std::size_t /*outputSize*/,
    int         /*frameWidth*/,
    int         /*frameHeight*/,
    float       /*confidenceThreshold*/,
    float       /*nmsThreshold*/) const
{
    // TODO: decode YOLO grid cells / anchors
    // TODO: filter by objectness * class_score > confidenceThreshold
    // TODO: apply NMS (cv::dnn::NMSBoxes)
    // TODO: scale boxes back to original frame dimensions
    return {};
}

std::string YoloDetectionStrategy::strategyName() const { return "yolo"; }

} // namespace spectra::vision::strategy
