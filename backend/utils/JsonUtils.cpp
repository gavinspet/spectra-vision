#include "JsonUtils.h"
#include <stdexcept>

namespace spectra::vision::utils {

Json::Value JsonUtils::parse(const std::string& raw) {
    Json::CharReaderBuilder builder;
    Json::Value             root;
    std::string             errs;
    // TODO: parse raw into root; throw std::invalid_argument on failure
    (void)raw;
    return root;
}

std::string JsonUtils::serialize(const Json::Value& value) {
    Json::StreamWriterBuilder builder;
    // TODO: return Json::writeString(builder, value)
    (void)value;
    return {};
}

std::string JsonUtils::requireString(const Json::Value& obj, const std::string& key) {
    // TODO: throw ValidationException if key missing or not a string
    return obj.get(key, "").asString();
}

float JsonUtils::requireFloat(const Json::Value& obj, const std::string& key) {
    // TODO: throw ValidationException if key missing or not numeric
    return obj.get(key, 0.0f).asFloat();
}

int JsonUtils::requireInt(const Json::Value& obj, const std::string& key) {
    return obj.get(key, 0).asInt();
}

std::string JsonUtils::optionalString(const Json::Value& obj,
                                      const std::string& key,
                                      const std::string& defaultVal) {
    return obj.get(key, defaultVal).asString();
}

} // namespace spectra::vision::utils
