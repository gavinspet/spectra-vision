#pragma once

#include <memory>
#include <string>

#include "LogLevel.h"

namespace spectra::vision::logger {

// ═════════════════════════════════════════════════════════════════════════════
// LOG CONFIG — Centralized logging configuration (immutable after init)
// ═════════════════════════════════════════════════════════════════════════════

/// Centralized logging configuration.
/// Immutable after construction to ensure consistency across application.
///
/// Design:
/// - NOT a singleton (injected where needed)
/// - Immutable after construction (const methods, no setters)
/// - Thread-safe (immutable data + shared_ptr ownership)
/// - Separate from Logger (separation of concerns)
///
/// Usage:
///   LogConfig config;
///   config.setConsoleOutput(true)
///       .setFileOutput("./logs/app.log")
///       .setLogLevel(LogLevel::INFO)
///       .initialize();
///
class LogConfig {
public:
    /// Creates a new log configuration with sensible defaults.
    LogConfig();

    /// Deleted copy (prevent accidental duplication)
    LogConfig(const LogConfig&) = delete;
    LogConfig& operator=(const LogConfig&) = delete;

    // ─────────────────────────────────────────────────────────────────────────
    // BUILDER-STYLE CONFIGURATION
    // ─────────────────────────────────────────────────────────────────────────

    /// Enables or disables console output
    /// @param enable true to enable console logging
    /// @return *this for method chaining
    LogConfig& setConsoleOutput(bool enable);

    /// Sets file output path
    /// @param filePath Path to log file (empty to disable)
    /// @return *this for method chaining
    LogConfig& setFileOutput(const std::string& filePath);

    /// Sets the global log level
    /// @param level Minimum level to log (messages below are filtered)
    /// @return *this for method chaining
    LogConfig& setLogLevel(LogLevel level);

    /// Enables or disables log rotation
    /// @param enable true to enable rotation
    /// @return *this for method chaining
    LogConfig& setLogRotation(bool enable);

    /// Sets the log file size limit before rotation
    /// @param bytes Size in bytes (default: 10MB)
    /// @return *this for method chaining
    LogConfig& setLogFileSizeLimit(std::size_t bytes);

    /// Sets the maximum number of rotated log files to keep
    /// @param count Number of files (default: 5)
    /// @return *this for method chaining
    LogConfig& setMaxLogFiles(int count);

    /// Initializes the logging system with current configuration
    /// @throws std::runtime_error if configuration is invalid
    void initialize();

    // ─────────────────────────────────────────────────────────────────────────
    // QUERY METHODS (const)
    // ─────────────────────────────────────────────────────────────────────────

    /// Checks if console output is enabled
    bool isConsoleOutputEnabled() const noexcept;

    /// Gets the file output path
    const std::string& getFileOutputPath() const noexcept;

    /// Gets the current log level
    LogLevel getLogLevel() const noexcept;

    /// Checks if log rotation is enabled
    bool isLogRotationEnabled() const noexcept;

    /// Gets the log file size limit
    std::size_t getLogFileSizeLimit() const noexcept;

    /// Gets the maximum number of log files
    int getMaxLogFiles() const noexcept;

    /// Checks if configuration has been initialized
    bool isInitialized() const noexcept;

private:
    // Configuration state
    bool consoleOutput_;
    std::string fileOutput_;
    LogLevel logLevel_;
    bool logRotation_;
    std::size_t logFileSizeLimit_;
    int maxLogFiles_;
    bool initialized_;

    /// Validates configuration consistency
    /// @throws std::invalid_argument if config is invalid
    void validate() const;

    /// Sets up Drogon/trantor logging based on configuration
    void setupTrantorLogger();
};

}  // namespace spectra::vision::logger
