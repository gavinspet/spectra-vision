#include "config/JsonParser.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace spectra::vision::config::detail {

// Helper macros for JSON field extraction
#define REQUIRE_STRING(obj, field)                                   \
    do {                                                             \
        if (!obj.isMember(field) || !obj[field].isString()) {       \
            throw std::invalid_argument("Missing or invalid field: " \
                                        + std::string(field));       \
        }                                                            \
    } while (false)

#define REQUIRE_INT(obj, field)                                      \
    do {                                                             \
        if (!obj.isMember(field) || !obj[field].isInt()) {          \
            throw std::invalid_argument("Missing or invalid field: " \
                                        + std::string(field));       \
        }                                                            \
    } while (false)

#define GET_STRING(obj, field, default_val) \
    (obj.isMember(field) && obj[field].isString() ? obj[field].asString() \
                                                   : default_val)

#define GET_INT(obj, field, default_val) \
    (obj.isMember(field) && obj[field].isInt() ? obj[field].asInt() \
                                               : default_val)

#define GET_FLOAT(obj, field, default_val) \
    (obj.isMember(field) && (obj[field].isDouble() || obj[field].isInt()) \
         ? static_cast<float>(obj[field].asDouble())                      \
         : default_val)

Json::Value loadJsonFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open config file: " + filePath);
    }

    Json::Value root;
    std::string errs;
    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;

    if (!Json::parseFromStream(builder, file, &root, &errs)) {
        throw std::runtime_error("Invalid JSON in " + filePath + ": " + errs);
    }

    return root;
}

ServerConfig parseServerConfig(const Json::Value& root) {
    if (!root.isMember("server")) {
        throw std::invalid_argument("Missing 'server' section");
    }

    const auto& serverJson = root["server"];
    ServerConfig cfg;

    cfg.host = GET_STRING(serverJson, "host", "0.0.0.0");
    cfg.port = GET_INT(serverJson, "port", 8080);
    cfg.threadCount = GET_INT(serverJson, "threadCount", 16);
    cfg.logPath = GET_STRING(serverJson, "logPath", "./logs");
    cfg.logLevel = GET_STRING(serverJson, "logLevel", "info");

    if (cfg.port == 0 || cfg.port > 65535) {
        throw std::invalid_argument("Invalid port number: " +
                                    std::to_string(cfg.port));
    }
    if (cfg.threadCount < 1) {
        throw std::invalid_argument(
            "Thread count must be at least 1, got: " +
            std::to_string(cfg.threadCount));
    }

    return cfg;
}

SecurityConfig parseSecurityConfig(const Json::Value& root) {
    SecurityConfig cfg;

    if (!root.isMember("security")) {
        return cfg;  // Use defaults if section missing
    }

    const auto& secJson = root["security"];
    cfg.enableAuth = secJson.isMember("enableAuth")
                         ? secJson["enableAuth"].asBool()
                         : true;
    cfg.rateLimitRps = GET_INT(secJson, "rateLimitRps", 100);
    cfg.jwtSecret = GET_STRING(secJson, "jwtSecret", "");

    if (cfg.rateLimitRps < 1) {
        throw std::invalid_argument(
            "Rate limit must be positive, got: " +
            std::to_string(cfg.rateLimitRps));
    }

    return cfg;
}

PreprocessingConfig parsePreprocessingConfig(const Json::Value& root) {
    PreprocessingConfig cfg;

    if (!root.isMember("preprocessing")) {
        return cfg;  // Use defaults if section missing
    }

    const auto& prepJson = root["preprocessing"];
    cfg.targetWidth = GET_INT(prepJson, "targetWidth", 224);
    cfg.targetHeight = GET_INT(prepJson, "targetHeight", 224);
    cfg.normalizeScale = GET_FLOAT(prepJson, "normalizeScale", 1.0f / 255.0f);
    cfg.meanR = GET_FLOAT(prepJson, "meanR", 0.0f);
    cfg.meanG = GET_FLOAT(prepJson, "meanG", 0.0f);
    cfg.meanB = GET_FLOAT(prepJson, "meanB", 0.0f);

    return cfg;
}

InputShape parseInputShape(const Json::Value& shapeJson) {
    InputShape shape;

    shape.batch = GET_INT(shapeJson, "batch", 1);
    shape.channels = GET_INT(shapeJson, "channels", 3);
    shape.height = GET_INT(shapeJson, "height", 640);
    shape.width = GET_INT(shapeJson, "width", 640);

    if (shape.batch < 1 || shape.channels < 1 || shape.height < 1 ||
        shape.width < 1) {
        throw std::invalid_argument("Input shape dimensions must be positive");
    }

    return shape;
}

ModelConfig parseModelConfig(const Json::Value& modelJson) {
    REQUIRE_STRING(modelJson, "modelId");
    REQUIRE_STRING(modelJson, "modelPath");
    REQUIRE_STRING(modelJson, "modelType");
    REQUIRE_STRING(modelJson, "detectionStrategy");

    if (!modelJson.isMember("classLabels") ||
        !modelJson["classLabels"].isArray()) {
        throw std::invalid_argument("Missing or invalid 'classLabels'");
    }

    ModelConfig cfg;
    cfg.modelId = modelJson["modelId"].asString();
    cfg.modelPath = modelJson["modelPath"].asString();
    cfg.modelType = modelJson["modelType"].asString();
    cfg.detectionStrategy = modelJson["detectionStrategy"].asString();

    // Parse class labels
    const auto& labelsJson = modelJson["classLabels"];
    for (const auto& label : labelsJson) {
        if (!label.isString()) {
            throw std::invalid_argument("Class labels must be strings");
        }
        cfg.classLabels.push_back(label.asString());
    }

    // Parse input shape
    if (modelJson.isMember("inputShape")) {
        cfg.inputShape = parseInputShape(modelJson["inputShape"]);
    }

    cfg.confidenceThreshold =
        GET_FLOAT(modelJson, "confidenceThreshold", 0.5f);
    cfg.nmsThreshold = GET_FLOAT(modelJson, "nmsThreshold", 0.45f);

    if (cfg.confidenceThreshold < 0.0f || cfg.confidenceThreshold > 1.0f) {
        throw std::invalid_argument(
            "Confidence threshold must be in [0, 1], got: " +
            std::to_string(cfg.confidenceThreshold));
    }
    if (cfg.nmsThreshold < 0.0f || cfg.nmsThreshold > 1.0f) {
        throw std::invalid_argument("NMS threshold must be in [0, 1], got: " +
                                    std::to_string(cfg.nmsThreshold));
    }

    return cfg;
}

ModelRegistry parseModelRegistry(const Json::Value& root) {
    if (!root.isMember("models")) {
        throw std::invalid_argument("Missing 'models' section");
    }

    const auto& modelsJson = root["models"];
    if (!modelsJson.isArray()) {
        throw std::invalid_argument("'models' must be an array");
    }

    std::vector<ModelConfig> models;
    models.reserve(modelsJson.size());

    for (const auto& modelJson : modelsJson) {
        models.push_back(parseModelConfig(modelJson));
    }

    return ModelRegistry(std::move(models));
}

}  // namespace spectra::vision::config::detail
