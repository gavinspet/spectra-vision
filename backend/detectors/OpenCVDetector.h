#pragma once

#include <memory>
#include <string>
#include "interfaces/IDetector.h"
#include "strategy/IDetectionStrategy.h"

namespace cv::dnn { class Net; }

namespace spectra::vision::detectors {

// Infrastructure Layer — OpenCV DNN backend (Caffe, Darknet, TF, ONNX via OpenCV).
// Secondary backend; can be swapped transparently via the factory.

class OpenCVDetector : public interfaces::IDetector {
public:
    explicit OpenCVDetector(std::shared_ptr<strategy::IDetectionStrategy> strategy);
    ~OpenCVDetector() override;

    void loadModel(const std::string& modelPath) override;
    std::vector<dto::BoundingBox> detect(const models::Frame& frame) override;
    std::string getBackendName() const override;
    bool isReady() const override;

private:
    std::unique_ptr<cv::dnn::Net>                 net_;
    std::shared_ptr<strategy::IDetectionStrategy> strategy_;
    bool ready_{false};
};

} // namespace spectra::vision::detectors
