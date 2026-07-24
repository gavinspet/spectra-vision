#pragma once

#include "IInferenceEngine.h"
#include <memory>
#include <string>
#include <vector>
#include <mutex>

namespace Ort { class Session; class Env; }

namespace spectra::vision::inference {

// Infrastructure Layer — ONNX Runtime inference engine implementation.
// Concrete backend using ONNX Runtime for model loading and inference.
//
// Design Principles:
// 1. Opaque Pointers: ONNX Runtime types (Ort::Session, Ort::Env, OrtValue) are
//    forward-declared in header but only accessed in .cpp. Zero impact on clients.
//
// 2. RAII Resource Management: unique_ptr<Ort::Session> and static Ort::Env.
//    Exception in constructor is safe (unique_ptr destruct cleans up).
//    No manual cleanup, no leaked resources even if exceptions occur.
//
// 3. Thread-Safe Inference: ONNX Runtime sessions are inherently thread-safe per spec.
//    Multiple threads can call infer() concurrently without additional locks.
//    Model loading is exclusive (caller must serialize load/unload if needed).
//
// 4. Move Semantics: Move-constructible and move-assignable (implements Rule of Five).
//    Deep copying an ONNX session is expensive, so moves make ownership transfer explicit.
//    Copy constructor is deleted to prevent accidental deep copies.
//
// 5. Exception Translation: ONNX Runtime errors (OrtException) are caught and
//    rethrown as DetectionException. Caller never sees vendor-specific types.

class OnnxInferenceEngine final : public IInferenceEngine {
public:
    OnnxInferenceEngine();
    ~OnnxInferenceEngine();

    OnnxInferenceEngine(const OnnxInferenceEngine&) = delete;
    OnnxInferenceEngine& operator=(const OnnxInferenceEngine&) = delete;

    OnnxInferenceEngine(OnnxInferenceEngine&& other) noexcept;
    OnnxInferenceEngine& operator=(OnnxInferenceEngine&& other) noexcept;

    void loadModel(const std::string& modelPath) override;
    void unload() noexcept override;
    bool isLoaded() const noexcept override;

    Tensor infer(const Tensor& input) override;

    std::vector<int64_t> getInputShape() const noexcept override;
    std::vector<int64_t> getOutputShape() const noexcept override;
    std::string getModelName() const noexcept override;

private:
    std::unique_ptr<Ort::Session> session_;
    std::vector<int64_t>          inputShape_;
    std::vector<int64_t>          outputShape_;
    std::string                   modelName_;
    mutable std::mutex            mutex_;

    // Cached input/output node names to avoid repeated string allocations.
    std::string inputNodeName_;
    std::string outputNodeName_;

    void validateInput(const Tensor& input) const;
};

} // namespace spectra::vision::inference
