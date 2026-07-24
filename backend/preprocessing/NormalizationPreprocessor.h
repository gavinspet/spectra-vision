#pragma once

#include "interfaces/IPreprocessor.h"

namespace spectra::vision::preprocessing {

// Infrastructure Layer — subtracts mean and scales pixel values to [0, 1]
// or any model-specific normalisation scheme.

class NormalizationPreprocessor : public interfaces::IPreprocessor {
public:
    NormalizationPreprocessor(float scale = 1.0f / 255.0f,
                               float meanR = 0.0f,
                               float meanG = 0.0f,
                               float meanB = 0.0f);

    models::Frame preprocess(const models::Frame& input) const override;
    void configure(int targetWidth, int targetHeight) override;
    std::string name() const override;

private:
    float scale_{1.0f / 255.0f};
    float meanR_{0.0f};
    float meanG_{0.0f};
    float meanB_{0.0f};
};

} // namespace spectra::vision::preprocessing
