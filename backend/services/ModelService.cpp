#include "ModelService.h"
#include "dto/ModelInfoDto.h"

namespace spectra::vision::services {

ModelService::ModelService(std::shared_ptr<factory::DetectorFactory> detectorFactory)
    : factory_(std::move(detectorFactory)) {}

void ModelService::registerModel(const std::string& modelId,
                                 const std::string& modelPath,
                                 const std::string& modelType) {
    // TODO: delegate to factory_, store in registry_
}

void ModelService::unloadModel(const std::string& modelId) {
    // TODO: erase from registry_
}

std::vector<dto::ModelInfoDto> ModelService::listModels() const {
    // TODO: iterate registry_, map to ModelInfoDto
    return {};
}

bool ModelService::isModelLoaded(const std::string& modelId) const {
    return registry_.count(modelId) > 0;
}

std::shared_ptr<interfaces::IDetector>
ModelService::getDetector(const std::string& modelId) const {
    // TODO: look up registry_, throw if missing
    return nullptr;
}

} // namespace spectra::vision::services
