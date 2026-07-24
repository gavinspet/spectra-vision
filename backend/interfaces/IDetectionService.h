#pragma once

namespace spectra::vision::dto { struct DetectionRequest; struct DetectionResponse; }

namespace spectra::vision::interfaces {

// Application Layer — the primary use-case boundary for object detection.
// Controllers depend on this abstraction, never on the concrete service.
class IDetectionService {
public:
    virtual ~IDetectionService() = default;

    virtual dto::DetectionResponse detect(const dto::DetectionRequest& request) = 0;
};

} // namespace spectra::vision::interfaces
