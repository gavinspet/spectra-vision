#pragma once

#include <string>
#include <vector>
#include "models/ModelConfig.h"

namespace spectra::vision::config {

// Infrastructure Layer — reads the list of model artefacts from the config file
// and makes them available to ModelService on startup.

class ModelRegistryConfig {
public:
    static ModelRegistryConfig& instance();

    void load(const std::string& configFilePath);

    const std::vector<models::ModelConfig>& models() const noexcept;

private:
    ModelRegistryConfig() = default;

    std::vector<models::ModelConfig> models_;
};

} // namespace spectra::vision::config
