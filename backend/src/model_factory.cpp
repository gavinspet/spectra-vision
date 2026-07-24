#include "model_factory.hpp"
#include "logger.hpp"

namespace sv {

std::shared_ptr<EmotionDetectionStrategy> EmotionModelFactory::create(const std::string& modelId) {
    auto& logger = Logger::getInstance();
    logger.info("Creating emotion detector for model: " + modelId);
    
    if (modelId == "emotion-v1" || modelId == "mock") {
        return std::make_shared<MockEmotionDetector>();
    }
    else if (modelId == "emotion-v2" || modelId == "advanced") {
        return std::make_shared<AdvancedEmotionDetector>();
    }
    else {
        logger.error("Unknown model ID: " + modelId);
        throw ModelException("Unknown model ID: " + modelId);
    }
}

std::vector<std::string> EmotionModelFactory::listAvailableModels() {
    return {
        "emotion-v1",  // Alias: mock
        "emotion-v2",  // Alias: advanced
        "mock",
        "advanced"
    };
}

} // namespace sv
