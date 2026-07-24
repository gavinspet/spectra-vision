#pragma once

#include <fmt/format.h>

#include "Logger.h"

namespace spectra::vision::logger {

// ═════════════════════════════════════════════════════════════════════════════
// LOGGER TEMPLATE IMPLEMENTATIONS
// ═════════════════════════════════════════════════════════════════════════════

/// Why templates in a header?
/// - Avoids function call overhead for common cases
/// - Enables compiler to inline format operations
/// - Allows compile-time type checking for format arguments
/// - Only instantiated when actually used

template <typename... Args>
void Logger::trace(std::string_view fmt, Args&&... args) {
    if (!isEnabled(LogLevel::TRACE)) {
        return;  // Early return if log level disabled (zero-cost when disabled)
    }
    try {
        auto formatted = fmt::format(fmt, std::forward<Args>(args)...);
        logImplFormatted(LogLevel::TRACE, fmt, formatted);
    } catch (const fmt::format_error& e) {
        // Format error — log the error itself
        logImpl(LogLevel::ERROR,
               "Format error in trace log: " + std::string(e.what()));
    }
}

template <typename... Args>
void Logger::debug(std::string_view fmt, Args&&... args) {
    if (!isEnabled(LogLevel::DEBUG)) {
        return;
    }
    try {
        auto formatted = fmt::format(fmt, std::forward<Args>(args)...);
        logImplFormatted(LogLevel::DEBUG, fmt, formatted);
    } catch (const fmt::format_error& e) {
        logImpl(LogLevel::ERROR,
               "Format error in debug log: " + std::string(e.what()));
    }
}

template <typename... Args>
void Logger::info(std::string_view fmt, Args&&... args) {
    if (!isEnabled(LogLevel::INFO)) {
        return;
    }
    try {
        auto formatted = fmt::format(fmt, std::forward<Args>(args)...);
        logImplFormatted(LogLevel::INFO, fmt, formatted);
    } catch (const fmt::format_error& e) {
        logImpl(LogLevel::ERROR, "Format error in info log: " +
                                    std::string(e.what()));
    }
}

template <typename... Args>
void Logger::warn(std::string_view fmt, Args&&... args) {
    if (!isEnabled(LogLevel::WARN)) {
        return;
    }
    try {
        auto formatted = fmt::format(fmt, std::forward<Args>(args)...);
        logImplFormatted(LogLevel::WARN, fmt, formatted);
    } catch (const fmt::format_error& e) {
        logImpl(LogLevel::ERROR,
               "Format error in warn log: " + std::string(e.what()));
    }
}

template <typename... Args>
void Logger::error(std::string_view fmt, Args&&... args) {
    if (!isEnabled(LogLevel::ERROR)) {
        return;
    }
    try {
        auto formatted = fmt::format(fmt, std::forward<Args>(args)...);
        logImplFormatted(LogLevel::ERROR, fmt, formatted);
    } catch (const fmt::format_error& e) {
        logImpl(LogLevel::ERROR,
               "Format error in error log: " + std::string(e.what()));
    }
}

template <typename... Args>
void Logger::fatal(std::string_view fmt, Args&&... args) {
    // FATAL always logs (no level check)
    try {
        auto formatted = fmt::format(fmt, std::forward<Args>(args)...);
        logImplFormatted(LogLevel::FATAL, fmt, formatted);
    } catch (const fmt::format_error& e) {
        logImpl(LogLevel::FATAL,
               "Format error in fatal log: " + std::string(e.what()));
    }
}

}  // namespace spectra::vision::logger
