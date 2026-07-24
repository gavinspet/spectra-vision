#include "ImagePreprocessor.h"
#include "models/Frame.h"

namespace spectra::vision::preprocessing {

ImagePreprocessor::ImagePreprocessor() = default;

models::Frame ImagePreprocessor::preprocess(const models::Frame& input) const {
    // TODO: delegate to ResizePreprocessor then NormalizationPreprocessor
    return input;
}

void ImagePreprocessor::configure(int targetWidth, int targetHeight) {
    targetWidth_  = targetWidth;
    targetHeight_ = targetHeight;
}

std::string ImagePreprocessor::name() const { return "ImagePreprocessor"; }

} // namespace spectra::vision::preprocessing
