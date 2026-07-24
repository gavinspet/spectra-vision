#pragma once

#include <memory>
#include <string_view>

#include "Logger.h"
#include "LogConfig.h"

namespace spectra::vision::logger {

// ═════════════════════════════════════════════════════════════════════════════
// LOGGER FACTORY — Creates loggers with consistent configuration
// ═════════════════════════════════════════════════════════════════════════════

/// Factory for creating Logger instances with consistent configuration.
///
/// Design Rationale:
/// - Centralizes logger creation (enforces conventions)
/// - Ensures all loggers use same configuration
/// - Enables global features (e.g., component filtering)
/// - Not a singleton — injected where needed
/// - Thread-safe (immutable after construction)
///
/// Usage:
///   auto config = std::make_shared<LogConfig>();
///   config->setLogLevel(LogLevel::INFO)->initialize();
///   
///   LoggerFactory factory(config);
///   auto logger = factory.create("DetectionService");
///   logger.info("Processing frame");
///
class LoggerFactory {
public:
    /// Constructs factory with configuration
    /// @param config Shared configuration for all created loggers
    /// @throws std::invalid_argument if config is null or not initialized
    explicit LoggerFactory(std::shared_ptr<const LogConfig> config);

    /// Deleted copy (prevent accidental duplication)
    LoggerFactory(const LoggerFactory&) = delete;
    LoggerFactory& operator=(const LoggerFactory&) = delete;

    /// Move constructor (efficient transfer)
    LoggerFactory(LoggerFactory&&) noexcept = default;

    /// Move assignment (efficient transfer)
    LoggerFactory& operator=(LoggerFactory&&) noexcept = default;

    /// Destructor
    ~LoggerFactory() = default;

    /// Creates a logger with the given component name
    /// @param component Component name (e.g., "DetectionService")
    /// @return Logger instance
    /// @throws std::invalid_argument if component is empty
    Logger create(std::string_view component) const;

    /// Gets the shared configuration
    /// @return Const reference to configuration
    const LogConfig& getConfig() const noexcept;

private:
    std::shared_ptr<const LogConfig> config_;
};

}  // namespace spectra::vision::logger
