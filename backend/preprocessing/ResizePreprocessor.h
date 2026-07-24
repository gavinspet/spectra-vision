#pragma once

#include "interfaces/IPreprocessor.h"

namespace spectra::vision::preprocessing {

// Infrastructure Layer — resizes the frame to the model's expected input dimensions
// using letterboxing to preserve aspect ratio.

class ResizePreprocessor : public interfaces::IPreprocessor {
public:
    ResizePreprocessor(int targetWidth, int targetHeight);

    models::Frame preprocess(const models::Frame& input) const override;
    void configure(int targetWidth, int targetHeight) override;
    std::string name() const override;

private:
    int targetWidth_;
    int targetHeight_;
};

} // namespace spectra::vision::preprocessing
