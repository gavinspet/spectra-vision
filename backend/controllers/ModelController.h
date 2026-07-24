#pragma once

#include <drogon/HttpController.h>
#include <memory>
#include "interfaces/IModelService.h"

namespace spectra::vision::controllers {

// Presentation Layer — CRUD operations on registered model artefacts.
// Consumed by ops tooling and the management UI.

class ModelController
    : public drogon::HttpController<ModelController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(ModelController::listModels,
                      "/api/v1/models",
                      drogon::Get,
                      "spectra::vision::middleware::AuthMiddleware");
        ADD_METHOD_TO(ModelController::registerModel,
                      "/api/v1/models",
                      drogon::Post,
                      "spectra::vision::middleware::AuthMiddleware");
        ADD_METHOD_TO(ModelController::unloadModel,
                      "/api/v1/models/{modelId}",
                      drogon::Delete,
                      "spectra::vision::middleware::AuthMiddleware");
    METHOD_LIST_END

    explicit ModelController(
        std::shared_ptr<interfaces::IModelService> modelService);

    void listModels(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void registerModel(const drogon::HttpRequestPtr& req,
                       std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void unloadModel(const drogon::HttpRequestPtr& req,
                     std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                     const std::string& modelId);

private:
    std::shared_ptr<interfaces::IModelService> modelService_;
};

} // namespace spectra::vision::controllers
