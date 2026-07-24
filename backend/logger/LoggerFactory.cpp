#include "logger/LoggerFactory.h"

#include <stdexcept>

namespace spectra::vision::logger {

// ─────────────────────────────────────────────────────────────────────────────
// LOGGERFACTORY IMPLEMENTATION
// ─────────────────────────────────────────────────────────────────────────────

LoggerFactory::LoggerFactory(std::shared_ptr<const LogConfig> config)
    : config_(std::move(config)) {
    if (!config_) {
        throw std::invalid_argument("LogConfig cannot be null");
    }
    if (!config_->isInitialized()) {
        throw std::invalid_argument("LogConfig must be initialized");
    }
}

Logger LoggerFactory::create(std::string_view component) const {
    if (component.empty()) {
        throw std::invalid_argument("Component name cannot be empty");
    }
    return Logger(component);
}

const LogConfig& LoggerFactory::getConfig() const noexcept { return *config_; }

}  // namespace spectra::vision::logger
