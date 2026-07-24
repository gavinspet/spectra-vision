#pragma once

#include "emotion_detection.hpp"
#include <memory>
#include <string>
#include <map>
#include <vector>

namespace sv {

/**
 * @brief Factory Pattern: Creates appropriate emotion detection strategy
 * 
 * The factory encapsulates the logic of creating the right detector based
 * on the model ID, following the Factory design pattern.
 */
class EmotionModelFactory {
public:
    /**
     * @brief Create an emotion detector for the given model ID
     * @param modelId Unique identifier for the model
     * @return Shared pointer to the emotion detection strategy
     * @throws ModelException if model ID is not recognized
     */
    static std::shared_ptr<EmotionDetectionStrategy> create(const std::string& modelId);
    
    /**
     * @brief List all available models
     * @return Vector of model IDs
     */
    static std::vector<std::string> listAvailableModels();

private:
    // Private constructor - static factory only
    EmotionModelFactory() = delete;
};

} // namespace sv
