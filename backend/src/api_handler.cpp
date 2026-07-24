#include "api_handler.hpp"

namespace sv {

json APIHandler::handleEmotionDetection(const json& jsonRequest) {
    auto& logger = Logger::getInstance();
    
    try {
        validateEmotionRequest(jsonRequest);
        
        std::string modelId = jsonRequest["modelId"].get<std::string>();
        std::string imageBase64 = jsonRequest["imageBase64"].get<std::string>();
        
        logger.info("Processing emotion detection request for model: " + modelId);
        
        // Factory creates the appropriate strategy
        auto detector = EmotionModelFactory::create(modelId);
        
        // Strategy pattern: call the appropriate detector
        auto result = detector->detect(imageBase64);
        
        json response = {
            {"classLabel", result.classLabel},
            {"classId", result.classId},
            {"confidence", result.confidence},
            {"inferenceTimeMs", result.inferenceTimeMs},
            {"timestamp", result.timestamp}
        };
        
        logger.info("Emotion detection completed successfully");
        return response;
        
    } catch (const Exception& e) {
        logger.error(e.what());
        return json{
            {"error", e.what()},
            {"status", "error"}
        };
    } catch (const std::exception& e) {
        logger.critical("Unexpected error: " + std::string(e.what()));
        return json{
            {"error", "Internal server error"},
            {"status", "error"}
        };
    }
}

json APIHandler::handleHealthCheck() const {
    return json{
        {"status", "ok"},
        {"service", "Spectra Vision"},
        {"version", "2.0.0"},
        {"buildType", "Release"}
    };
}

json APIHandler::handleListModels() const {
    auto models = EmotionModelFactory::listAvailableModels();
    return json{
        {"models", models},
        {"count", models.size()}
    };
}

void APIHandler::validateEmotionRequest(const json& request) {
    auto& logger = Logger::getInstance();
    
    if (!request.contains("modelId")) {
        logger.warning("Missing required field: modelId");
        throw Exception("Missing required field: modelId");
    }
    
    if (!request.contains("imageBase64")) {
        logger.warning("Missing required field: imageBase64");
        throw Exception("Missing required field: imageBase64");
    }
    
    if (!request["modelId"].is_string()) {
        logger.warning("Invalid type for modelId: expected string");
        throw Exception("Invalid type for modelId: expected string");
    }
    
    if (!request["imageBase64"].is_string()) {
        logger.warning("Invalid type for imageBase64: expected string");
        throw Exception("Invalid type for imageBase64: expected string");
    }
}

} // namespace sv
