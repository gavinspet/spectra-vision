#pragma once

#include <memory>
#include "interfaces/IDetectionService.h"
#include "interfaces/IDetector.h"
#include "interfaces/IPreprocessingService.h"

namespace spectra::vision::services {

// Application Layer — orchestrates the full detection pipeline.
// Delegates preprocessing to IPreprocessingService and inference to IDetector.
// Controllers depend on IDetectionService, not this concrete type.

class DetectionService : public interfaces::IDetectionService {
public:
    DetectionService(std::shared_ptr<interfaces::IDetector>           detector,
                     std::shared_ptr<interfaces::IPreprocessingService> preprocessor);

    dto::DetectionResponse detect(const dto::DetectionRequest& request) override;

private:
    std::shared_ptr<interfaces::IDetector>            detector_;
    std::shared_ptr<interfaces::IPreprocessingService> preprocessor_;
};

} // namespace spectra::vision::services
