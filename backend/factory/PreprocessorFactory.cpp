#include "PreprocessorFactory.h"

namespace spectra::vision::factory {

void PreprocessorFactory::registerPreprocessor(const std::string& key, CreatorFn creator) {
    registry_[key] = std::move(creator);
}

std::shared_ptr<interfaces::IPreprocessor>
PreprocessorFactory::create(const std::string& key,
                            const models::ModelConfig& config) const {
    auto it = registry_.find(key);
    if (it == registry_.end()) {
        return nullptr;
    }
    return it->second(config);
}

} // namespace spectra::vision::factory
