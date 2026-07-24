#pragma once

namespace spectra::vision::models { class Frame; }

namespace spectra::vision::interfaces {

// Application Layer — runs an ordered preprocessing pipeline on a raw frame
// before it is forwarded to the detector.
class IPreprocessingService {
public:
    virtual ~IPreprocessingService() = default;

    virtual models::Frame runPipeline(const models::Frame& raw) const = 0;
};

} // namespace spectra::vision::interfaces
