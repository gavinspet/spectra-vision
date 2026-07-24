#pragma once

#include <memory>
#include <string>

namespace spectra::vision::interfaces { class IDetector; }

namespace spectra::vision::interfaces {

// Infrastructure Layer — abstracts how a model artefact is loaded from disk.
// Concrete loaders (ONNX, OpenCV DNN, TensorRT) implement this and register
// themselves with DetectorFactory.
class IModelLoader {
public:
    virtual ~IModelLoader() = default;

    virtual std::shared_ptr<IDetector> load(const std::string& modelPath,
                                            const std::string& modelType) = 0;
    virtual bool supports(const std::string& modelType) const = 0;
};

} // namespace spectra::vision::interfaces
