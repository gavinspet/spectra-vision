#include "DetectorFactory.h"
#include "exceptions/ModelLoadException.h"

namespace spectra::vision::factory {

void DetectorFactory::registerBackend(const std::string& backendKey, CreatorFn creator) {
    registry_[backendKey] = std::move(creator);
}

std::shared_ptr<interfaces::IDetector>
DetectorFactory::create(const models::ModelConfig& config) const {
    auto it = registry_.find(config.modelType);
    if (it == registry_.end()) {
        throw exceptions::ModelLoadException(config.modelPath,
            "No backend registered for type: " + config.modelType);
    }
    auto detector = it->second(config);
    detector->loadModel(config.modelPath);
    return detector;
}

bool DetectorFactory::hasBackend(const std::string& backendKey) const {
    return registry_.count(backendKey) > 0;
}

} // namespace spectra::vision::factory
