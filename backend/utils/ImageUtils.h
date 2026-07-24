#pragma once

#include <string>
#include <vector>

// Forward-declare cv::Mat to avoid pulling in all of OpenCV in every TU.
namespace cv { class Mat; }
namespace spectra::vision::models { class Frame; }

namespace spectra::vision::utils {

// Shared Utilities — OpenCV Mat ↔ Frame conversions and image I/O helpers.
// Pure functions; no state; safe to call from any thread.

class ImageUtils {
public:
    ImageUtils() = delete;

    static models::Frame matToFrame(const cv::Mat& mat);
    static cv::Mat       frameToMat(const models::Frame& frame);

    static models::Frame decodeBase64Image(const std::string& base64Data);
    static std::string   encodeFrameToBase64(const models::Frame& frame);
};

} // namespace spectra::vision::utils
