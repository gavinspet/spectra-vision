#pragma once

#include <string>
#include <vector>

// Forward declarations — avoids tight coupling to concrete types.
namespace spectra::vision::models { class Frame; }
namespace spectra::vision::dto    { struct BoundingBox; }

namespace spectra::vision::interfaces {

// Domain Layer — defines the contract every detector backend must satisfy.
// Implementations live in detectors/; the factory selects the concrete type.
class IDetector {
public:
    virtual ~IDetector() = default;

    virtual void loadModel(const std::string& modelPath) = 0;
    virtual std::vector<dto::BoundingBox> detect(const models::Frame& frame) = 0;
    virtual std::string getBackendName() const = 0;
    virtual bool isReady() const = 0;
};

} // namespace spectra::vision::interfaces
