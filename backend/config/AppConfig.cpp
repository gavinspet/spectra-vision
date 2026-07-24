#include "config/AppConfig.h"

#include "config/JsonParser.h"

namespace spectra::vision::config {

AppConfig::AppConfig(ServerConfig srv, SecurityConfig sec,
                     PreprocessingConfig prep, ModelRegistry mdl)
    : server_(std::move(srv)), security_(std::move(sec)),
      preprocessing_(std::move(prep)), models_(std::move(mdl)) {}

std::shared_ptr<const AppConfig> AppConfig::load(
    const std::string& configFilePath) {
    // Parse JSON file
    const auto json = detail::loadJsonFile(configFilePath);

    // Parse each configuration section
    auto server = detail::parseServerConfig(json);
    auto security = detail::parseSecurityConfig(json);
    auto preprocessing = detail::parsePreprocessingConfig(json);
    auto models = detail::parseModelRegistry(json);

    // Create AppConfig with all parsed configurations
    // Using private constructor via new + shared_ptr
    auto config = std::make_shared<AppConfig>(
        std::move(server), std::move(security), std::move(preprocessing),
        std::move(models));

    return config;
}

const ServerConfig& AppConfig::server() const noexcept { return server_; }

const SecurityConfig& AppConfig::security() const noexcept {
    return security_;
}

const PreprocessingConfig& AppConfig::preprocessing() const noexcept {
    return preprocessing_;
}

const ModelRegistry& AppConfig::models() const noexcept { return models_; }

}  // namespace spectra::vision::config
