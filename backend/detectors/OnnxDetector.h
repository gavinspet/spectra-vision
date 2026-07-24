#pragma once

#include <memory>
#include <string>
#include "interfaces/IDetector.h"
#include "strategy/IDetectionStrategy.h"

// onnxruntime forward declarations
namespace Ort { class Session; class Env; }

namespace spectra::vision::detectors {

// Infrastructure Layer — ONNX Runtime backend for model inference.
// Implements IDetector; instantiated by DetectorFactory.

class OnnxDetector : public interfaces::IDetector {
public:
    OnnxDetector();
    explicit OnnxDetector(std::shared_ptr<strategy::IDetectionStrategy> strategy);
    ~OnnxDetector() override;

    void loadModel(const std::string& modelPath) override;
    std::vector<dto::BoundingBox> detect(const models::Frame& frame) override;
    std::string getBackendName() const override;
    bool isReady() const override;

private:
    std::unique_ptr<Ort::Env>     env_;
    std::unique_ptr<Ort::Session> session_;
    std::shared_ptr<strategy::IDetectionStrategy> strategy_;
    bool ready_{false};
};

} // namespace spectra::vision::detectors
