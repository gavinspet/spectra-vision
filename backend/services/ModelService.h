#pragma once

#include <memory>
#include <unordered_map>
#include "interfaces/IModelService.h"
#include "interfaces/IDetector.h"
#include "factory/DetectorFactory.h"

namespace spectra::vision::services {

// Application Layer — owns the registry of loaded detector instances.
// Decouples controller-level model management from concrete detector creation.

class ModelService : public interfaces::IModelService {
public:
    explicit ModelService(std::shared_ptr<factory::DetectorFactory> detectorFactory);

    void registerModel(const std::string& modelId,
                       const std::string& modelPath,
                       const std::string& modelType) override;

    void unloadModel(const std::string& modelId) override;

    std::vector<dto::ModelInfoDto> listModels() const override;

    bool isModelLoaded(const std::string& modelId) const override;

    std::shared_ptr<interfaces::IDetector> getDetector(const std::string& modelId) const;

private:
    std::shared_ptr<factory::DetectorFactory>                      factory_;
    std::unordered_map<std::string, std::shared_ptr<interfaces::IDetector>> registry_;
};

} // namespace spectra::vision::services
