#include "ModelRegistryConfig.h"

namespace spectra::vision::config {

ModelRegistryConfig& ModelRegistryConfig::instance() {
    static ModelRegistryConfig inst;
    return inst;
}

void ModelRegistryConfig::load(const std::string& configFilePath) {
    // TODO: parse "models" JSON array from configFilePath
    // TODO: deserialise each entry into models::ModelConfig
    // TODO: push_back into models_
    (void)configFilePath;
}

const std::vector<models::ModelConfig>& ModelRegistryConfig::models() const noexcept {
    return models_;
}

} // namespace spectra::vision::config
