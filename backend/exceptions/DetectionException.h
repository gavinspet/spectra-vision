#pragma once

#include "SpectraException.h"

namespace spectra::vision::exceptions {

// Domain Layer — thrown when inference fails or produces invalid output.
class DetectionException : public SpectraException {
public:
    explicit DetectionException(const std::string& message)
        : SpectraException("DetectionException: " + message, 500) {}
};

} // namespace spectra::vision::exceptions
