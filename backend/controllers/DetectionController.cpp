#include "DetectionController.h"
#include "dto/DetectionRequest.h"
#include "dto/DetectionResponse.h"
#include "exceptions/ValidationException.h"
#include "exceptions/SpectraException.h"
#include <drogon/HttpResponse.h>
#include <json/json.h>

namespace spectra::vision::controllers {

DetectionController::DetectionController(
    std::shared_ptr<interfaces::IDetectionService> service)
    : service_(std::move(service)) {}

void DetectionController::detect(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    // TODO: parse JSON body → dto::DetectionRequest
    // TODO: validate required fields (throw ValidationException on failure)
    // TODO: service_->detect(request)
    // TODO: serialise dto::DetectionResponse → JSON response
    // TODO: catch SpectraException, map to appropriate HTTP status
    auto resp = drogon::HttpResponse::newHttpJsonResponse({});
    callback(resp);
}

} // namespace spectra::vision::controllers
