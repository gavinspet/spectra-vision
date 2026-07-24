#pragma once

#include <string>

#include <json/json.h>

#include "ConfigTypes.h"
#include "ModelRegistry.h"

namespace spectra::vision::config::detail {

// ═════════════════════════════════════════════════════════════════════════════
// JSON PARSING UTILITIES (internal)
// ═════════════════════════════════════════════════════════════════════════════

/// Loads JSON from file.
/// @param filePath Path to JSON file
/// @return Parsed JSON root value
/// @throws std::runtime_error if file not found or JSON is malformed
Json::Value loadJsonFile(const std::string& filePath);

/// Parses server configuration from JSON.
/// @param root Root JSON object
/// @return Parsed ServerConfig
/// @throws std::invalid_argument if required fields missing
ServerConfig parseServerConfig(const Json::Value& root);

/// Parses security configuration from JSON.
/// @param root Root JSON object
/// @return Parsed SecurityConfig
/// @throws std::invalid_argument if required fields missing
SecurityConfig parseSecurityConfig(const Json::Value& root);

/// Parses preprocessing configuration from JSON.
/// @param root Root JSON object
/// @return Parsed PreprocessingConfig (uses defaults if missing)
PreprocessingConfig parsePreprocessingConfig(const Json::Value& root);

/// Parses a single model configuration from JSON.
/// @param modelJson JSON object for a model
/// @return Parsed ModelConfig
/// @throws std::invalid_argument if required fields missing
ModelConfig parseModelConfig(const Json::Value& modelJson);

/// Parses input shape from JSON.
/// @param shapeJson JSON object for input shape
/// @return Parsed InputShape
InputShape parseInputShape(const Json::Value& shapeJson);

/// Parses model registry (array of model configs) from JSON.
/// @param root Root JSON object
/// @return Constructed ModelRegistry
/// @throws std::invalid_argument on validation errors
ModelRegistry parseModelRegistry(const Json::Value& root);

}  // namespace spectra::vision::config::detail
