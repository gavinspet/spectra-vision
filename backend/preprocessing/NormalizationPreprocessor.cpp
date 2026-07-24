#include "NormalizationPreprocessor.h"
#include "models/Frame.h"

namespace spectra::vision::preprocessing {

NormalizationPreprocessor::NormalizationPreprocessor(float scale,
                                                     float meanR,
                                                     float meanG,
                                                     float meanB)
    : scale_(scale), meanR_(meanR), meanG_(meanG), meanB_(meanB) {}

models::Frame NormalizationPreprocessor::preprocess(const models::Frame& input) const {
    // TODO: pixel = (pixel - mean) * scale for each channel
    return input;
}

void NormalizationPreprocessor::configure(int /*targetWidth*/, int /*targetHeight*/) {
    // Normalisation has no spatial parameters; configure via constructor.
}

std::string NormalizationPreprocessor::name() const { return "NormalizationPreprocessor"; }

} // namespace spectra::vision::preprocessing
