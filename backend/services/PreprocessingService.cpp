#include "PreprocessingService.h"
#include "models/Frame.h"

namespace spectra::vision::services {

PreprocessingService::PreprocessingService(
    std::vector<std::shared_ptr<interfaces::IPreprocessor>> pipeline)
    : pipeline_(std::move(pipeline)) {}

models::Frame PreprocessingService::runPipeline(const models::Frame& raw) const {
    // TODO: fold raw through each step: frame = step->preprocess(frame)
    return raw;
}

void PreprocessingService::addStep(std::shared_ptr<interfaces::IPreprocessor> step) {
    pipeline_.push_back(std::move(step));
}

} // namespace spectra::vision::services
