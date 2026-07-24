#pragma once

#include "types.hpp"
#include <string>
#include <memory>

namespace sv {

/**
 * @brief Strategy Pattern: Abstract interface for emotion detection algorithms
 * 
 * This interface allows for different emotion detection strategies to be
 * plugged in at runtime without modifying the client code.
 */
class EmotionDetectionStrategy {
public:
    virtual ~EmotionDetectionStrategy() = default;
    
    /**
     * @brief Detect emotion from image data
     * @param imageData Base64 encoded image or path
     * @return EmotionResult with detected emotion and confidence
     * @throws InferenceException if detection fails
     */
    virtual EmotionResult detect(const std::string& imageData) = 0;
};

/**
 * @brief Concrete implementation: Mock emotion detection (for demo)
 */
class MockEmotionDetector : public EmotionDetectionStrategy {
public:
    EmotionResult detect(const std::string& imageData) override;
};

/**
 * @brief Concrete implementation: Advanced emotion detection with confidence variation
 */
class AdvancedEmotionDetector : public EmotionDetectionStrategy {
public:
    EmotionResult detect(const std::string& imageData) override;

private:
    // Simulate different emotions based on input
    std::string simulateEmotionLabel(const std::string& imageData);
    double simulateConfidence();
};

} // namespace sv
