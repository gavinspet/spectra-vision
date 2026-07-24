#pragma once

#include "emotion_detection.hpp"
#include "model_factory.hpp"
#include "logger.hpp"
#include <string>
#include <nlohmann/json.hpp>

namespace sv {

using json = nlohmann::json;

/**
 * @brief HTTP request handler for emotion detection API
 * 
 * Demonstrates:
 * - Dependency Injection (receives EmotionDetectionStrategy instances)
 * - Error handling and recovery
 * - JSON serialization/deserialization
 */
class APIHandler {
public:
    /**
     * @brief Handle emotion detection request
     * @param jsonRequest JSON request body with modelId and imageBase64
     * @return JSON response with emotion detection results
     */
    json handleEmotionDetection(const json& jsonRequest);
    
    /**
     * @brief Get server health status
     * @return JSON with health information
     */
    json handleHealthCheck() const;
    
    /**
     * @brief Get available models
     * @return JSON with list of available models
     */
    json handleListModels() const;

private:
    void validateEmotionRequest(const json& request);
};

} // namespace sv
