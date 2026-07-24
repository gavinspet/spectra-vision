#pragma once

#include <string>
#include <json/json.h>

namespace spectra::vision::utils {

// Shared Utilities — safe JSON parsing helpers used across controllers.
// Throws std::invalid_argument on malformed input rather than returning
// partial objects.

class JsonUtils {
public:
    JsonUtils() = delete;

    static Json::Value parse(const std::string& raw);
    static std::string serialize(const Json::Value& value);

    static std::string  requireString (const Json::Value& obj, const std::string& key);
    static float        requireFloat  (const Json::Value& obj, const std::string& key);
    static int          requireInt    (const Json::Value& obj, const std::string& key);
    static std::string  optionalString(const Json::Value& obj, const std::string& key,
                                       const std::string& defaultVal = "");
};

} // namespace spectra::vision::utils
