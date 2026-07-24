#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include "interfaces/IPreprocessor.h"
#include "models/ModelConfig.h"

namespace spectra::vision::factory {

// Infrastructure Layer — creates and configures preprocessing step instances.
// Allows swapping preprocessing implementations without touching service code.

class PreprocessorFactory {
public:
    using CreatorFn = std::function<std::shared_ptr<interfaces::IPreprocessor>(
        const models::ModelConfig&)>;

    void registerPreprocessor(const std::string& key, CreatorFn creator);

    std::shared_ptr<interfaces::IPreprocessor> create(
        const std::string& key, const models::ModelConfig& config) const;

private:
    std::unordered_map<std::string, CreatorFn> registry_;
};

} // namespace spectra::vision::factory
