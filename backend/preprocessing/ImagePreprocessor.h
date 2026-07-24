#pragma once

#include <string>
#include <vector>
#include "interfaces/IPreprocessor.h"

namespace spectra::vision::preprocessing {

// Infrastructure Layer — coordinates resize + normalisation into a single step.
// Can be used standalone or as a step inside PreprocessingService.

class ImagePreprocessor : public interfaces::IPreprocessor {
public:
    ImagePreprocessor();

    models::Frame preprocess(const models::Frame& input) const override;
    void configure(int targetWidth, int targetHeight) override;
    std::string name() const override;

private:
    int targetWidth_{640};
    int targetHeight_{640};
};

} // namespace spectra::vision::preprocessing
