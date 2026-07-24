#include "RouteRegistrar.h"
#include "controllers/EmotionController.h"
#include "services/DetectionService.h"
#include "detectors/OnnxDetector.h"
#include "preprocessing/ImagePreprocessor.h"

namespace spectra::vision::routes {

void RouteRegistrar::registerAll(drogon::HttpAppFramework& app) {
    registerApiV1(app);
    registerHealthRoutes(app);
}

void RouteRegistrar::registerApiV1(drogon::HttpAppFramework& app) {
    auto detector = std::make_shared<detectors::OnnxDetector>();
    auto preprocessor = std::make_shared<preprocessing::ImagePreprocessor>();
    auto detectionService = std::make_shared<services::DetectionService>(
        detector, preprocessor);

    auto emotionController = std::make_shared<controllers::EmotionController>(
        detectionService);

    app.registerHttpController(emotionController);
}

void RouteRegistrar::registerHealthRoutes(drogon::HttpAppFramework& /*app*/) {
}

} // namespace spectra::vision::routes
