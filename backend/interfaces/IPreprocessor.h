#pragma once

namespace spectra::vision::models { class Frame; }

namespace spectra::vision::interfaces {

// Domain Layer — contract for every preprocessing step in the pipeline.
// Implementations live in preprocessing/ and are composed by PreprocessingService.
class IPreprocessor {
public:
    virtual ~IPreprocessor() = default;

    virtual models::Frame preprocess(const models::Frame& input) const = 0;
    virtual void configure(int targetWidth, int targetHeight) = 0;
    virtual std::string name() const = 0;
};

} // namespace spectra::vision::interfaces
