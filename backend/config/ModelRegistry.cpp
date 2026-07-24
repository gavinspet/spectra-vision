#include "config/ModelRegistry.h"

#include <algorithm>
#include <stdexcept>

namespace spectra::vision::config {

ModelRegistry::ModelRegistry(std::vector<ModelConfig> models)
    : models_(std::move(models)) {
    validate();
}

std::optional<std::reference_wrapper<const ModelConfig>>
ModelRegistry::getModel(const std::string& modelId) const {
    auto it = std::find_if(models_.begin(), models_.end(),
                           [&modelId](const ModelConfig& cfg) {
                               return cfg.modelId == modelId;
                           });

    if (it != models_.end()) {
        return std::cref(*it);
    }
    return std::nullopt;
}

bool ModelRegistry::hasModel(const std::string& modelId) const {
    return std::any_of(models_.begin(), models_.end(),
                       [&modelId](const ModelConfig& cfg) {
                           return cfg.modelId == modelId;
                       });
}

std::vector<std::string> ModelRegistry::listModelIds() const {
    std::vector<std::string> ids;
    ids.reserve(models_.size());
    for (const auto& model : models_) {
        ids.push_back(model.modelId);
    }
    return ids;
}

const std::vector<ModelConfig>& ModelRegistry::allModels() const {
    return models_;
}

std::size_t ModelRegistry::modelCount() const { return models_.size(); }

void ModelRegistry::validate() const {
    // Check for duplicate model IDs
    std::vector<std::string> ids;
    ids.reserve(models_.size());

    for (const auto& model : models_) {
        // Validate individual model
        if (model.modelId.empty()) {
            throw std::invalid_argument("Model ID cannot be empty");
        }
        if (model.modelPath.empty()) {
            throw std::invalid_argument("Model path cannot be empty for: " +
                                        model.modelId);
        }
        if (model.modelType.empty()) {
            throw std::invalid_argument("Model type cannot be empty for: " +
                                        model.modelId);
        }
        if (model.classLabels.empty()) {
            throw std::invalid_argument("Class labels cannot be empty for: " +
                                        model.modelId);
        }

        // Check for duplicate ID
        if (std::find(ids.begin(), ids.end(), model.modelId) != ids.end()) {
            throw std::invalid_argument("Duplicate model ID: " + model.modelId);
        }
        ids.push_back(model.modelId);
    }
}

}  // namespace spectra::vision::config
