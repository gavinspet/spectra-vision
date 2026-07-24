#pragma once

#include "Tensor.h"
#include <string>
#include <vector>
#include <memory>

namespace spectra::vision::inference {

// Infrastructure Layer — abstract inference engine interface.
// Defines the contract for pluggable backends (ONNX Runtime, TensorRT, etc.).
//
// Design Principles:
// 1. Backend-Independent API: All types are standard C++ or our Tensor wrapper.
//    No ONNX Runtime, TensorRT, or vendor-specific types in this interface.
//    This enables adding new backends without changing code that depends on IInferenceEngine.
//
// 2. Single Responsibility: Manages model loading and inference only.
//    Preprocessing, postprocessing, and result extraction are caller's responsibility.
//
// 3. Thread-Safe Inference: Multiple threads can call infer() concurrently.
//    Model loading and unloading are exclusive (caller must serialize if needed).
//
// 4. Exception-Based Error Handling: All failures throw SpectraException-derived exceptions.
//    No error codes, no out parameters for status.
//
// 5. Resource Management: RAII via unique_ptr for implementations.
//    No cleanup callbacks or manual destruction needed.

class IInferenceEngine {
public:
    virtual ~IInferenceEngine() = default;

    // Load model from filesystem path.
    // Throws DetectionException on file not found, parse error, or ONNX loading failure.
    // After successful load, isLoaded() returns true.
    virtual void loadModel(const std::string& modelPath) = 0;

    // Unload the currently loaded model, freeing GPU/CPU resources.
    // Safe to call when no model is loaded (no-op).
    // After unload, isLoaded() returns false.
    virtual void unload() noexcept = 0;

    // Check if a model is currently loaded.
    // Cheap operation, no I/O.
    virtual bool isLoaded() const noexcept = 0;

    // Run inference on input tensor and return output tensor.
    // Input must match model's input shape (shape validation is backend-specific).
    // Throws DetectionException if input shape is invalid.
    // Thread-safe: multiple threads can call infer() concurrently.
    // No model must be loaded before calling (throws DetectionException if !isLoaded()).
    virtual Tensor infer(const Tensor& input) = 0;

    // Get input shape expected by the loaded model.
    // Returns empty vector if no model loaded.
    // Shape format: [batch, height, width, channels] or model-specific.
    // Caller must understand the model's input layout.
    virtual std::vector<int64_t> getInputShape() const noexcept = 0;

    // Get output shape produced by the loaded model.
    // Returns empty vector if no model loaded.
    // Shape format is model-specific (e.g., [batch, num_classes] or [batch, height, width, channels]).
    // Caller must understand the model's output layout.
    virtual std::vector<int64_t> getOutputShape() const noexcept = 0;

    // Get human-readable model name or identifier.
    // Usually the filename without extension or a model metadata field.
    // Returns empty string if no model loaded.
    virtual std::string getModelName() const noexcept = 0;
};

using InferenceEnginePtr = std::unique_ptr<IInferenceEngine>;

} // namespace spectra::vision::inference
