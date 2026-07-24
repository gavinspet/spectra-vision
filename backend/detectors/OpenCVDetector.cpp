#include "OpenCVDetector.h"
#include "dto/BoundingBox.h"
#include "models/Frame.h"
#include "exceptions/ModelLoadException.h"
#include "exceptions/DetectionException.h"

namespace spectra::vision::detectors {

OpenCVDetector::OpenCVDetector(std::shared_ptr<strategy::IDetectionStrategy> strategy)
    : strategy_(std::move(strategy)) {}

OpenCVDetector::~OpenCVDetector() = default;

void OpenCVDetector::loadModel(const std::string& modelPath) {
    // TODO: cv::dnn::readNet(modelPath)
    // TODO: net_->setPreferableBackend / setPreferableTarget
    // TODO: set ready_ = true
}

std::vector<dto::BoundingBox> OpenCVDetector::detect(const models::Frame& frame) {
    // TODO: cv::dnn::blobFromImage
    // TODO: net_->setInput / forward
    // TODO: delegate to strategy_->postprocess(...)
    return {};
}

std::string OpenCVDetector::getBackendName() const { return "opencv-dnn"; }
bool        OpenCVDetector::isReady()        const { return ready_; }

} // namespace spectra::vision::detectors
