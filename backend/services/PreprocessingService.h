#pragma once

#include <memory>
#include <vector>
#include "interfaces/IPreprocessingService.h"
#include "interfaces/IPreprocessor.h"

namespace spectra::vision::services {

// Application Layer — chains an ordered sequence of IPreprocessor steps.
// Steps are injected at construction; order is significant.

class PreprocessingService : public interfaces::IPreprocessingService {
public:
    explicit PreprocessingService(
        std::vector<std::shared_ptr<interfaces::IPreprocessor>> pipeline);

    models::Frame runPipeline(const models::Frame& raw) const override;

    void addStep(std::shared_ptr<interfaces::IPreprocessor> step);

private:
    std::vector<std::shared_ptr<interfaces::IPreprocessor>> pipeline_;
};

} // namespace spectra::vision::services
