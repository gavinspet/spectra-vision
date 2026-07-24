#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "ConfigTypes.h"

namespace spectra::vision::config {

// ═════════════════════════════════════════════════════════════════════════════
// MODEL REGISTRY — Immutable, thread-safe model configuration repository
// ═════════════════════════════════════════════════════════════════════════════

/// Thread-safe registry of model configurations (immutable after construction).
/// Models are accessed by modelId and provide all inference parameters.
class ModelRegistry {
public:
    /// Constructs from a vector of model configurations.
    /// @param models List of model configurations
    /// @throws std::invalid_argument if duplicate modelIds or invalid config
    explicit ModelRegistry(std::vector<ModelConfig> models);

    /// Move constructor (efficient transfer of ownership)
    ModelRegistry(ModelRegistry&&) = default;

    /// Move assignment operator
    ModelRegistry& operator=(ModelRegistry&&) = default;

    /// Copy is not allowed (immutable registry should not be copied around)
    ModelRegistry(const ModelRegistry&) = delete;
    ModelRegistry& operator=(const ModelRegistry&) = delete;

    /// Retrieves a model configuration by ID.
    /// @param modelId The unique model identifier
    /// @return Reference to the model configuration, or std::nullopt if not found
    std::optional<std::reference_wrapper<const ModelConfig>>
    getModel(const std::string& modelId) const;

    /// Checks if a model exists by ID.
    /// @param modelId The unique model identifier
    /// @return True if model exists, false otherwise
    bool hasModel(const std::string& modelId) const;

    /// Lists all registered model IDs.
    /// @return Vector of model IDs in registration order
    std::vector<std::string> listModelIds() const;

    /// Lists all registered models.
    /// @return Reference to internal model list
    const std::vector<ModelConfig>& allModels() const;

    /// Gets the number of registered models.
    /// @return Count of models
    std::size_t modelCount() const;

private:
    std::vector<ModelConfig> models_;

    /// Validates registry for consistency (no duplicate IDs, valid configs)
    void validate() const;
};

}  // namespace spectra::vision::config
