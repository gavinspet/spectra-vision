#pragma once

#include <string>
#include <memory>
#include <stdexcept>

namespace sv {

// Domain models
struct EmotionResult {
    std::string classLabel;
    int classId;
    double confidence;
    int inferenceTimeMs;
    std::string timestamp;
};

// Exception hierarchy
class Exception : public std::runtime_error {
public:
    explicit Exception(const std::string& message) 
        : std::runtime_error(message) {}
};

class ModelException : public Exception {
public:
    explicit ModelException(const std::string& message) 
        : Exception("ModelException: " + message) {}
};

class InferenceException : public Exception {
public:
    explicit InferenceException(const std::string& message) 
        : Exception("InferenceException: " + message) {}
};

} // namespace sv
