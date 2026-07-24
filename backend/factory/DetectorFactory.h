#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include "interfaces/IDetector.h"
#include "models/ModelConfig.h"

namespace spectra::vision::factory {

// Infrastructure Layer — central creation point for IDetector instances.
// Backends (ONNX, OpenCV) register themselves with a string key.
// ModelService calls create() without knowing the concrete type.

class DetectorFactory {
public:
    using CreatorFn = std::function<std::shared_ptr<interfaces::IDetector>(
        const models::ModelConfig&)>;

    void registerBackend(const std::string& backendKey, CreatorFn creator);

    std::shared_ptr<interfaces::IDetector> create(
        const models::ModelConfig& config) const;

    bool hasBackend(const std::string& backendKey) const;

private:
    std::unordered_map<std::string, CreatorFn> registry_;
};

} // namespace spectra::vision::factory
