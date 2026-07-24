#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "LogLevel.h"

namespace spectra::vision::logger {

// ═════════════════════════════════════════════════════════════════════════════
// LOGGER CLASS — Main logging interface
// ═════════════════════════════════════════════════════════════════════════════

/// Production-grade logger with component-based naming, multiple levels,
/// and support for console/file output.
///
/// Design Principles:
/// 1. **Component-based**: Each logger has a name (component) for filtering
/// 2. **No global state**: Logger is injected where needed
/// 3. **Thread-safe**: Underlying trantor::Logger handles synchronization
/// 4. **Efficient**: Only formats strings if log level is enabled
/// 5. **Flexible format**: ISO timestamp, bracket notation
/// 6. **Movable**: Can be passed around efficiently
///
/// Example:
///   Logger logger("DetectionService");
///   logger.info("Loading model {}", modelName);
///   logger.error("Failed to load: {}", error.what());
///
class Logger {
public:
    /// Constructs a logger with the given component name.
    /// @param component Name of the component (e.g., "DetectionService", "ModelService")
    /// @throws std::invalid_argument if component is empty
    explicit Logger(std::string_view component);

    /// Move constructor (efficient transfer)
    Logger(Logger&&) noexcept = default;

    /// Move assignment (efficient transfer)
    Logger& operator=(Logger&&) noexcept = default;

    /// Copy constructor (loggers can be copied)
    Logger(const Logger&) = default;

    /// Copy assignment (loggers can be copied)
    Logger& operator=(const Logger&) = default;

    /// Destructor
    ~Logger() = default;

    // ─────────────────────────────────────────────────────────────────────────
    // LOG LEVEL METHODS — Variadic templates for format strings
    // ─────────────────────────────────────────────────────────────────────────

    /// Logs a TRACE-level message (entry/exit, detailed flow).
    /// Only logs if global log level <= TRACE (typically disabled).
    /// @param fmt Format string (supporting {} placeholders for args)
    /// @param args Arguments to format into the message
    template <typename... Args>
    void trace(std::string_view fmt, Args&&... args);

    /// Logs a DEBUG-level message (method parameters, state changes).
    /// Only logs if global log level <= DEBUG (typically disabled).
    template <typename... Args>
    void debug(std::string_view fmt, Args&&... args);

    /// Logs an INFO-level message (initialization, high-level flow).
    /// Only logs if global log level <= INFO (default: enabled).
    template <typename... Args>
    void info(std::string_view fmt, Args&&... args);

    /// Logs a WARN-level message (potential issues, recoverable errors).
    /// Only logs if global log level <= WARN (default: enabled).
    template <typename... Args>
    void warn(std::string_view fmt, Args&&... args);

    /// Logs an ERROR-level message (failed operations, exceptions).
    /// Only logs if global log level <= ERROR (default: enabled).
    template <typename... Args>
    void error(std::string_view fmt, Args&&... args);

    /// Logs a FATAL-level message (critical failures, must shutdown).
    /// Always logs regardless of level (for critical errors).
    template <typename... Args>
    void fatal(std::string_view fmt, Args&&... args);

    // ─────────────────────────────────────────────────────────────────────────
    // QUERY METHODS
    // ─────────────────────────────────────────────────────────────────────────

    /// Gets the component name for this logger
    /// @return Component name (e.g., "DetectionService")
    std::string_view getComponent() const noexcept;

    /// Checks if logging is enabled for a given level
    /// @param level The log level to check
    /// @return true if messages at this level will be logged
    static bool isEnabled(LogLevel level) noexcept;

private:
    std::string component_;

    /// Implementation for all log methods
    /// @param level The log level
    /// @param message The formatted message
    void logImpl(LogLevel level, std::string_view message) noexcept;

    /// Implementation for format strings with arguments
    /// @param level The log level
    /// @param fmt Format string
    /// @param args Formatting arguments
    void logImplFormatted(LogLevel level, std::string_view fmt,
                         const std::string& formatted) noexcept;
};

}  // namespace spectra::vision::logger

// Include implementation header for templates
#include "LoggerImpl.h"
