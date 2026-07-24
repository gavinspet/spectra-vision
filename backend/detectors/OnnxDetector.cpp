#include "OnnxDetector.h"
#include "dto/BoundingBox.h"
#include "models/Frame.h"
#include "exceptions/ModelLoadException.h"
#include "exceptions/DetectionException.h"

namespace spectra::vision::detectors {

OnnxDetector::OnnxDetector() : strategy_(nullptr) {}

OnnxDetector::OnnxDetector(std::shared_ptr<strategy::IDetectionStrategy> strategy)
    : strategy_(std::move(strategy)) {}

OnnxDetector::~OnnxDetector() = default;

void OnnxDetector::loadModel(const std::string& modelPath) {
}

std::vector<dto::BoundingBox> OnnxDetector::detect(const models::Frame& frame) {
    return {};
}

std::string OnnxDetector::getBackendName() const { return "onnxruntime"; }
bool        OnnxDetector::isReady()        const { return ready_; }

} // namespace spectra::vision::detectors
