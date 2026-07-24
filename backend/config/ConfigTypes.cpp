#include "config/ConfigTypes.h"

namespace spectra::vision::config {

bool ModelConfig::operator==(const ModelConfig& other) const {
    return modelId == other.modelId && modelPath == other.modelPath &&
           modelType == other.modelType &&
           detectionStrategy == other.detectionStrategy &&
           classLabels == other.classLabels &&
           inputShape.batch == other.inputShape.batch &&
           inputShape.channels == other.inputShape.channels &&
           inputShape.height == other.inputShape.height &&
           inputShape.width == other.inputShape.width &&
           confidenceThreshold == other.confidenceThreshold &&
           nmsThreshold == other.nmsThreshold;
}

}  // namespace spectra::vision::config
