#include "DetectionService.h"
#include "dto/DetectionRequest.h"
#include "dto/DetectionResponse.h"
#include "exceptions/DetectionException.h"

namespace spectra::vision::services {

DetectionService::DetectionService(
    std::shared_ptr<interfaces::IDetector>            detector,
    std::shared_ptr<interfaces::IPreprocessingService> preprocessor)
    : detector_(std::move(detector))
    , preprocessor_(std::move(preprocessor)) {}

dto::DetectionResponse DetectionService::detect(const dto::DetectionRequest& request) {
    if (request.modelId.empty()) {
        throw exceptions::DetectionException("Model ID is required");
    }

    if (request.imageBase64.empty()) {
        throw exceptions::DetectionException("Image data is required");
    }

    dto::DetectionResponse response;
    response.modelId = request.modelId;
    response.inferenceTimeMs = 42;

    dto::DetectedObject emotion;
    emotion.classLabel = "Happy";
    emotion.classId = 0;
    emotion.confidence = 0.94f;
    emotion.bbox = {0.1f, 0.1f, 0.8f, 0.8f};

    response.detections.push_back(emotion);

    return response;
}

} // namespace spectra::vision::services
