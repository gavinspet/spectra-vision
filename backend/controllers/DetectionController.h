#pragma once

#include <drogon/HttpController.h>
#include <memory>
#include "interfaces/IDetectionService.h"

namespace spectra::vision::controllers {

// Presentation Layer — handles all /api/v1/detect endpoints.
// Depends on IDetectionService; never touches infrastructure directly.

class DetectionController
    : public drogon::HttpController<DetectionController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(DetectionController::detect,
                      "/api/v1/detect",
                      drogon::Post,
                      "spectra::vision::middleware::AuthMiddleware",
                      "spectra::vision::middleware::LoggingMiddleware");
    METHOD_LIST_END

    explicit DetectionController(
        std::shared_ptr<interfaces::IDetectionService> service);

    void detect(const drogon::HttpRequestPtr& req,
                std::function<void(const drogon::HttpResponsePtr&)>&& callback);

private:
    std::shared_ptr<interfaces::IDetectionService> service_;
};

} // namespace spectra::vision::controllers
