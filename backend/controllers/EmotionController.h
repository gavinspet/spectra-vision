#pragma once

#include <drogon/HttpController.h>
#include <memory>
#include "interfaces/IDetectionService.h"

namespace spectra::vision::controllers {

class EmotionController : public drogon::HttpController<EmotionController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(EmotionController::recognize,
                      "/api/v1/emotion",
                      drogon::Post,
                      "spectra::vision::middleware::AuthMiddleware",
                      "spectra::vision::middleware::LoggingMiddleware");
    METHOD_LIST_END

    explicit EmotionController(
        std::shared_ptr<interfaces::IDetectionService> service);

    void recognize(const drogon::HttpRequestPtr& req,
                   std::function<void(const drogon::HttpResponsePtr&)>&& callback);

private:
    std::shared_ptr<interfaces::IDetectionService> service_;
};

} // namespace spectra::vision::controllers
