#pragma once

#include "SpectraException.h"

namespace spectra::vision::exceptions {

// Infrastructure Layer — thrown when a model file cannot be read or parsed.
class ModelLoadException : public SpectraException {
public:
    explicit ModelLoadException(const std::string& modelPath, const std::string& reason)
        : SpectraException("ModelLoadException [" + modelPath + "]: " + reason, 500) {}
};

} // namespace spectra::vision::exceptions
