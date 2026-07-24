#pragma once

#include <stdexcept>
#include <string>

namespace spectra::vision::exceptions {

// Domain Layer — base for all application-specific exceptions.
// Catch this type at the controller boundary to produce consistent error responses.

class SpectraException : public std::runtime_error {
public:
    explicit SpectraException(const std::string& message, int httpStatusCode = 500)
        : std::runtime_error(message), httpStatusCode_(httpStatusCode) {}

    int httpStatusCode() const noexcept { return httpStatusCode_; }

private:
    int httpStatusCode_;
};

} // namespace spectra::vision::exceptions
