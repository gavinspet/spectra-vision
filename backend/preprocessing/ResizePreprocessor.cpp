#include "ResizePreprocessor.h"
#include "models/Frame.h"

namespace spectra::vision::preprocessing {

ResizePreprocessor::ResizePreprocessor(int targetWidth, int targetHeight)
    : targetWidth_(targetWidth), targetHeight_(targetHeight) {}

models::Frame ResizePreprocessor::preprocess(const models::Frame& input) const {
    // TODO: cv::resize with letterboxing (keep aspect ratio, pad remainder)
    return input;
}

void ResizePreprocessor::configure(int targetWidth, int targetHeight) {
    targetWidth_  = targetWidth;
    targetHeight_ = targetHeight;
}

std::string ResizePreprocessor::name() const { return "ResizePreprocessor"; }

} // namespace spectra::vision::preprocessing
