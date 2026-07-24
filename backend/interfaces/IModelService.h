#pragma once

#include <string>
#include <vector>

namespace spectra::vision::dto { struct ModelInfoDto; }

namespace spectra::vision::interfaces {

// Application Layer — lifecycle management for registered model artefacts.
class IModelService {
public:
    virtual ~IModelService() = default;

    virtual void registerModel(const std::string& modelId,
                               const std::string& modelPath,
                               const std::string& modelType) = 0;
    virtual void unloadModel(const std::string& modelId) = 0;
    virtual std::vector<dto::ModelInfoDto> listModels() const = 0;
    virtual bool isModelLoaded(const std::string& modelId) const = 0;
};

} // namespace spectra::vision::interfaces
