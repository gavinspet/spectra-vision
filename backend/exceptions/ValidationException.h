#pragma once

#include "SpectraException.h"

namespace spectra::vision::exceptions {

// Presentation Layer — thrown when a controller receives a malformed request.
class ValidationException : public SpectraException {
public:
    explicit ValidationException(const std::string& field, const std::string& reason)
        : SpectraException("ValidationException [" + field + "]: " + reason, 400) {}
};

} // namespace spectra::vision::exceptions
