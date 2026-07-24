#include "logger/Logger.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <trantor/utils/Logger.h>

namespace spectra::vision::logger {

// Static variable to track global log level
static LogLevel g_logLevel = LogLevel::INFO;

// ─────────────────────────────────────────────────────────────────────────────
// LOGGER IMPLEMENTATION
// ─────────────────────────────────────────────────────────────────────────────

Logger::Logger(std::string_view component)
    : component_(component) {
    if (component.empty()) {
        throw std::invalid_argument("Component name cannot be empty");
    }
}

std::string_view Logger::getComponent() const noexcept { return component_; }

bool Logger::isEnabled(LogLevel level) noexcept {
    // Disable OFF level always, enable FATAL always
    if (level == LogLevel::OFF) {
        return false;
    }
    if (level == LogLevel::FATAL) {
        return true;
    }
    // Otherwise check against global level
    return static_cast<int>(level) >= static_cast<int>(g_logLevel);
}

void Logger::logImpl(LogLevel level, std::string_view message) noexcept {
    if (level == LogLevel::OFF) {
        return;
    }

    // Don't log if level is below minimum (except FATAL)
    if (level != LogLevel::FATAL &&
        static_cast<int>(level) < static_cast<int>(g_logLevel)) {
        return;
    }

    // Format timestamp in ISO 8601 format: YYYY-MM-DD HH:MM:SS
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) %
              1000;

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    std::string timestamp = oss.str();
    std::string levelStr = logLevelToString(level);

    // Format: [TIMESTAMP] [LEVEL] [COMPONENT] message
    std::string formatted = "[" + timestamp + "] [" + levelStr + "] [" +
                            component_ + "] " + std::string(message);

    // Use trantor::Logger for actual output
    switch (level) {
        case LogLevel::TRACE:
        case LogLevel::DEBUG:
            LOG_DEBUG << formatted;
            break;
        case LogLevel::INFO:
            LOG_INFO << formatted;
            break;
        case LogLevel::WARN:
            LOG_WARN << formatted;
            break;
        case LogLevel::ERROR:
            LOG_ERROR << formatted;
            break;
        case LogLevel::FATAL:
            LOG_FATAL << formatted;
            break;
        case LogLevel::OFF:
            break;
    }
}

void Logger::logImplFormatted(LogLevel level, std::string_view fmt,
                             const std::string& formatted) noexcept {
    // Just delegate to logImpl with the already-formatted message
    logImpl(level, formatted);
}

}  // namespace spectra::vision::logger
