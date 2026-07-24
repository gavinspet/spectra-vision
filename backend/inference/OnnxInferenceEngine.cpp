#include "OnnxInferenceEngine.h"
#include "exceptions/DetectionException.h"
#include <onnxruntime_cxx_api.h>
#include <filesystem>
#include <algorithm>

using namespace spectra::vision;

namespace {

static Ort::Env& getGlobalOnnxEnv() {
    static Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "OnnxInferenceEngine");
    return env;
}

std::string getModelBaseName(const std::string& path) {
    auto p = std::filesystem::path(path);
    return p.stem().string();
}

}

namespace spectra::vision::inference {

OnnxInferenceEngine::OnnxInferenceEngine()
    : session_(nullptr) {
}

OnnxInferenceEngine::~OnnxInferenceEngine() {
    unload();
}

OnnxInferenceEngine::OnnxInferenceEngine(OnnxInferenceEngine&& other) noexcept
    : session_(std::move(other.session_)),
      inputShape_(std::move(other.inputShape_)),
      outputShape_(std::move(other.outputShape_)),
      modelName_(std::move(other.modelName_)),
      inputNodeName_(std::move(other.inputNodeName_)),
      outputNodeName_(std::move(other.outputNodeName_)) {
}

OnnxInferenceEngine& OnnxInferenceEngine::operator=(OnnxInferenceEngine&& other) noexcept {
    if (this != &other) {
        session_ = std::move(other.session_);
        inputShape_ = std::move(other.inputShape_);
        outputShape_ = std::move(other.outputShape_);
        modelName_ = std::move(other.modelName_);
        inputNodeName_ = std::move(other.inputNodeName_);
        outputNodeName_ = std::move(other.outputNodeName_);
    }
    return *this;
}

void OnnxInferenceEngine::loadModel(const std::string& modelPath) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!std::filesystem::exists(modelPath)) {
        throw exceptions::DetectionException(
            "Model file not found: " + modelPath);
    }

    try {
        Ort::SessionOptions sessionOptions;
        sessionOptions.SetIntraOpNumThreads(1);
        sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

        auto& env = getGlobalOnnxEnv();
        session_ = std::make_unique<Ort::Session>(
            env, modelPath.c_str(), sessionOptions);

        modelName_ = getModelBaseName(modelPath);

        Ort::AllocatorWithDefaultOptions allocator;

        size_t inputCount = session_->GetInputCount();
        if (inputCount != 1) {
            throw exceptions::DetectionException(
                "Expected 1 input node, found: " + std::to_string(inputCount));
        }

        inputNodeName_ = session_->GetInputName(0, allocator);
        auto inputTypeInfo = session_->GetInputTypeInfo(0);
        auto inputTensorInfo = inputTypeInfo.GetTensorTypeAndShapeInfo();
        inputShape_ = inputTensorInfo.GetShape();

        size_t outputCount = session_->GetOutputCount();
        if (outputCount != 1) {
            throw exceptions::DetectionException(
                "Expected 1 output node, found: " + std::to_string(outputCount));
        }

        outputNodeName_ = session_->GetOutputName(0, allocator);
        auto outputTypeInfo = session_->GetOutputTypeInfo(0);
        auto outputTensorInfo = outputTypeInfo.GetTensorTypeAndShapeInfo();
        outputShape_ = outputTensorInfo.GetShape();

    } catch (const Ort::Exception& e) {
        session_.reset();
        inputShape_.clear();
        outputShape_.clear();
        modelName_.clear();
        inputNodeName_.clear();
        outputNodeName_.clear();
        throw exceptions::DetectionException(
            std::string("ONNX Runtime error: ") + e.what());
    } catch (const exceptions::SpectraException&) {
        session_.reset();
        inputShape_.clear();
        outputShape_.clear();
        modelName_.clear();
        inputNodeName_.clear();
        outputNodeName_.clear();
        throw;
    } catch (const std::exception& e) {
        session_.reset();
        inputShape_.clear();
        outputShape_.clear();
        modelName_.clear();
        inputNodeName_.clear();
        outputNodeName_.clear();
        throw exceptions::DetectionException(
            std::string("Unexpected error loading model: ") + e.what());
    }
}

void OnnxInferenceEngine::unload() noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    session_.reset();
    inputShape_.clear();
    outputShape_.clear();
    modelName_.clear();
    inputNodeName_.clear();
    outputNodeName_.clear();
}

bool OnnxInferenceEngine::isLoaded() const noexcept {
    return session_ != nullptr;
}

std::vector<int64_t> OnnxInferenceEngine::getInputShape() const noexcept {
    return inputShape_;
}

std::vector<int64_t> OnnxInferenceEngine::getOutputShape() const noexcept {
    return outputShape_;
}

std::string OnnxInferenceEngine::getModelName() const noexcept {
    return modelName_;
}

void OnnxInferenceEngine::validateInput(const Tensor& input) const {
    if (input.empty()) {
        throw exceptions::DetectionException("Input tensor is empty");
    }

    if (input.shape().size() != inputShape_.size()) {
        throw exceptions::DetectionException(
            "Input dimensionality mismatch: expected " +
            std::to_string(inputShape_.size()) + " dims, got " +
            std::to_string(input.shape().size()));
    }

    for (size_t i = 0; i < inputShape_.size(); ++i) {
        if (inputShape_[i] > 0 && inputShape_[i] != input.shape()[i]) {
            throw exceptions::DetectionException(
                "Input shape mismatch at dimension " + std::to_string(i) +
                ": expected " + std::to_string(inputShape_[i]) +
                ", got " + std::to_string(input.shape()[i]));
        }
    }
}

Tensor OnnxInferenceEngine::infer(const Tensor& input) {
    if (!isLoaded()) {
        throw exceptions::DetectionException("No model loaded");
    }

    validateInput(input);

    try {
        Ort::AllocatorWithDefaultOptions allocator;

        std::vector<const char*> inputNames{inputNodeName_.c_str()};
        std::vector<const char*> outputNames{outputNodeName_.c_str()};

        Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
            allocator,
            const_cast<float*>(input.data_ptr()),
            input.size(),
            input.shape().data(),
            input.shape().size());

        std::vector<Ort::Value> outputTensors = session_->Run(
            Ort::RunOptions{nullptr},
            inputNames.data(),
            &inputTensor,
            1,
            outputNames.data(),
            1);

        if (outputTensors.empty()) {
            throw exceptions::DetectionException("Inference produced no output");
        }

        auto& outputTensor = outputTensors[0];

        if (!outputTensor.IsTensor()) {
            throw exceptions::DetectionException("Output is not a tensor");
        }

        auto outputTensorInfo = outputTensor.GetTensorTypeAndShapeInfo();
        std::vector<int64_t> outShape = outputTensorInfo.GetShape();
        const float* outputData = outputTensor.GetTensorData<float>();
        size_t outputSize = outputTensorInfo.GetElementCount();

        std::vector<float> resultData(outputData, outputData + outputSize);
        return Tensor(outShape, std::move(resultData));

    } catch (const Ort::Exception& e) {
        throw exceptions::DetectionException(
            std::string("ONNX Runtime inference error: ") + e.what());
    } catch (const exceptions::SpectraException&) {
        throw;
    } catch (const std::exception& e) {
        throw exceptions::DetectionException(
            std::string("Unexpected inference error: ") + e.what());
    }
}

} // namespace spectra::vision::inference
