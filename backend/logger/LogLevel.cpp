#include "logger/LogLevel.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace spectra::vision::logger {

std::string logLevelToString(LogLevel level) noexcept {
    switch (level) {
        case LogLevel::TRACE:
            return "TRACE";
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARN:
            return "WARN";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::FATAL:
            return "FATAL";
        case LogLevel::OFF:
            return "OFF";
        default:
            return "UNKNOWN";
    }
}

LogLevel stringToLogLevel(const std::string& str) noexcept {
    // Convert to uppercase for comparison
    std::string upper = str;
    std::transform(upper.begin(), upper.end(), upper.begin(),
                   [](unsigned char c) { return std::toupper(c); });

    if (upper == "TRACE") {
        return LogLevel::TRACE;
    }
    if (upper == "DEBUG") {
        return LogLevel::DEBUG;
    }
    if (upper == "INFO") {
        return LogLevel::INFO;
    }
    if (upper == "WARN" || upper == "WARNING") {
        return LogLevel::WARN;
    }
    if (upper == "ERROR") {
        return LogLevel::ERROR;
    }
    if (upper == "FATAL") {
        return LogLevel::FATAL;
    }
    if (upper == "OFF") {
        return LogLevel::OFF;
    }

    // Default to INFO if invalid
    return LogLevel::INFO;
}

}  // namespace spectra::vision::logger
