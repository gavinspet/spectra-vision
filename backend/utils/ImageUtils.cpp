#include "ImageUtils.h"
#include "models/Frame.h"

namespace spectra::vision::utils {

models::Frame ImageUtils::matToFrame(const cv::Mat& /*mat*/) {
    // TODO: copy Mat pixel data into Frame::data_
    return {};
}

cv::Mat ImageUtils::frameToMat(const models::Frame& /*frame*/) {
    // TODO: wrap Frame::data_ in cv::Mat header (no copy)
    return {};
}

models::Frame ImageUtils::decodeBase64Image(const std::string& /*base64Data*/) {
    // TODO: base64 decode → raw bytes → cv::imdecode → matToFrame
    return {};
}

std::string ImageUtils::encodeFrameToBase64(const models::Frame& /*frame*/) {
    // TODO: frameToMat → cv::imencode → base64 encode
    return {};
}

} // namespace spectra::vision::utils
