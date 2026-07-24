#include "ModelController.h"
#include "exceptions/SpectraException.h"
#include <drogon/HttpResponse.h>

namespace spectra::vision::controllers {

ModelController::ModelController(
    std::shared_ptr<interfaces::IModelService> modelService)
    : modelService_(std::move(modelService)) {}

void ModelController::listModels(
    const drogon::HttpRequestPtr& /*req*/,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    // TODO: modelService_->listModels() → serialise to JSON array
    callback(drogon::HttpResponse::newHttpJsonResponse(Json::Value(Json::arrayValue)));
}

void ModelController::registerModel(
    const drogon::HttpRequestPtr& /*req*/,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    // TODO: parse body → modelId, modelPath, modelType
    // TODO: modelService_->registerModel(...)
    callback(drogon::HttpResponse::newHttpJsonResponse({}));
}

void ModelController::unloadModel(
    const drogon::HttpRequestPtr& /*req*/,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    const std::string& modelId)
{
    // TODO: modelService_->unloadModel(modelId)
    (void)modelId;
    callback(drogon::HttpResponse::newHttpJsonResponse({}));
}

} // namespace spectra::vision::controllers
