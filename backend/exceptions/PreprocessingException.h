#pragma once

#include "SpectraException.h"

namespace spectra::vision::exceptions {

// Infrastructure Layer — thrown when a preprocessing step fails (e.g., corrupt image data).
class PreprocessingException : public SpectraException {
public:
    explicit PreprocessingException(const std::string& step, const std::string& reason)
        : SpectraException("PreprocessingException [" + step + "]: " + reason, 422) {}
};

} // namespace spectra::vision::exceptions
