#include "HealthController.h"
#include <drogon/HttpResponse.h>

namespace spectra::vision::controllers {

void HealthController::asyncHandleHttpRequest(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    // TODO: differentiate /live (always 200) vs /ready (check model registry)
    Json::Value body;
    body["status"] = "ok";
    callback(drogon::HttpResponse::newHttpJsonResponse(body));
}

} // namespace spectra::vision::controllers
