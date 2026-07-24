#pragma once

#include <string>

namespace spectra::vision::logger {

// ═════════════════════════════════════════════════════════════════════════════
// LOG LEVEL ENUM
// ═════════════════════════════════════════════════════════════════════════════

/// Log severity levels in increasing order of severity.
/// Used to filter which messages are actually logged.
enum class LogLevel : int {
    /// Detailed information for debugging flow within the application
    /// Use for: Entry/exit of functions, variable values, loop iterations
    /// Default: DISABLED (too verbose for production)
    TRACE = 0,

    /// Information useful for debugging the application
    /// Use for: Method parameters, query results, state changes
    /// Default: DISABLED (verbose, enable in development)
    DEBUG = 1,

    /// Informational messages about application flow
    /// Use for: Module initialization, configuration, high-level flow
    /// Default: ENABLED (general operational events)
    INFO = 2,

    /// Warning about potential issues
    /// Use for: Deprecated API usage, recoverable errors, resource concerns
    /// Default: ENABLED (should be investigated)
    WARN = 3,

    /// Error that impairs functionality
    /// Use for: Failed operations, exception handling, data validation failures
    /// Default: ENABLED (needs immediate attention)
    ERROR = 4,

    /// Fatal error that forces shutdown
    /// Use for: Critical system failures, unrecoverable conditions
    /// Default: ENABLED (emergency only)
    FATAL = 5,

    /// Disable all logging
    /// Use for: Testing, performance-critical scenarios
    OFF = 6,
};

/// Converts LogLevel to string representation
/// @param level The log level
/// @return String representation (e.g., "INFO", "ERROR")
std::string logLevelToString(LogLevel level) noexcept;

/// Converts string to LogLevel
/// @param str The string to convert (case-insensitive)
/// @return LogLevel, or INFO if invalid
LogLevel stringToLogLevel(const std::string& str) noexcept;

}  // namespace spectra::vision::logger
