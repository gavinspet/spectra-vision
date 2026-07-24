#include "logger/LogConfig.h"

#include <filesystem>
#include <stdexcept>

#include <trantor/utils/Logger.h>

namespace spectra::vision::logger {

// ─────────────────────────────────────────────────────────────────────────────
// LOGCONFIG IMPLEMENTATION
// ─────────────────────────────────────────────────────────────────────────────

LogConfig::LogConfig()
    : consoleOutput_(true),
      fileOutput_(""),
      logLevel_(LogLevel::INFO),
      logRotation_(true),
      logFileSizeLimit_(10 * 1024 * 1024),  // 10 MB
      maxLogFiles_(5),
      initialized_(false) {}

LogConfig& LogConfig::setConsoleOutput(bool enable) {
    consoleOutput_ = enable;
    return *this;
}

LogConfig& LogConfig::setFileOutput(const std::string& filePath) {
    fileOutput_ = filePath;
    return *this;
}

LogConfig& LogConfig::setLogLevel(LogLevel level) {
    logLevel_ = level;
    return *this;
}

LogConfig& LogConfig::setLogRotation(bool enable) {
    logRotation_ = enable;
    return *this;
}

LogConfig& LogConfig::setLogFileSizeLimit(std::size_t bytes) {
    if (bytes == 0) {
        throw std::invalid_argument("Log file size limit must be > 0");
    }
    logFileSizeLimit_ = bytes;
    return *this;
}

LogConfig& LogConfig::setMaxLogFiles(int count) {
    if (count < 1) {
        throw std::invalid_argument("Max log files must be >= 1");
    }
    maxLogFiles_ = count;
    return *this;
}

void LogConfig::validate() const {
    // At least one output must be enabled
    if (!consoleOutput_ && fileOutput_.empty()) {
        throw std::invalid_argument(
            "At least one output (console or file) must be enabled");
    }

    // If file output is enabled, check path is valid
    if (!fileOutput_.empty()) {
        namespace fs = std::filesystem;
        try {
            // Create parent directories if they don't exist
            auto parent = fs::path(fileOutput_).parent_path();
            if (!parent.empty() && !fs::exists(parent)) {
                fs::create_directories(parent);
            }
        } catch (const fs::filesystem_error& e) {
            throw std::invalid_argument(
                std::string("Invalid log file path: ") + e.what());
        }
    }
}

void LogConfig::setupTrantorLogger() {
    // Configure trantor logger based on settings

    // Set output type
    if (consoleOutput_ && fileOutput_.empty()) {
        trantor::Logger::setOutputFunction(
            [](const char* msg, std::size_t len) {
                std::cout.write(msg, len);
                std::cout.flush();
            },
            [](const char* msg, std::size_t len) {
                std::cerr.write(msg, len);
                std::cerr.flush();
            });
    } else if (!fileOutput_.empty()) {
        // File output is typically handled by trantor's built-in mechanisms
        // For now, we'll use trantor's default file logging
        trantor::Logger::setLogFile(fileOutput_, "", logFileSizeLimit_);
    }

    // Set log level (map our LogLevel to trantor's)
    trantor::LogLevel trantorLevel = trantor::LogLevel::kInfo;
    switch (logLevel_) {
        case LogLevel::TRACE:
            trantorLevel = trantor::LogLevel::kDebug;
            break;
        case LogLevel::DEBUG:
            trantorLevel = trantor::LogLevel::kDebug;
            break;
        case LogLevel::INFO:
            trantorLevel = trantor::LogLevel::kInfo;
            break;
        case LogLevel::WARN:
            trantorLevel = trantor::LogLevel::kWarn;
            break;
        case LogLevel::ERROR:
            trantorLevel = trantor::LogLevel::kError;
            break;
        case LogLevel::FATAL:
            trantorLevel = trantor::LogLevel::kFatal;
            break;
        case LogLevel::OFF:
            // Disable all logging at trantor level
            trantorLevel = trantor::LogLevel::kFatal;
            break;
    }

    trantor::Logger::setLogLevel(trantorLevel);
}

void LogConfig::initialize() {
    validate();
    setupTrantorLogger();
    initialized_ = true;
}

bool LogConfig::isConsoleOutputEnabled() const noexcept {
    return consoleOutput_;
}

const std::string& LogConfig::getFileOutputPath() const noexcept {
    return fileOutput_;
}

LogLevel LogConfig::getLogLevel() const noexcept { return logLevel_; }

bool LogConfig::isLogRotationEnabled() const noexcept { return logRotation_; }

std::size_t LogConfig::getLogFileSizeLimit() const noexcept {
    return logFileSizeLimit_;
}

int LogConfig::getMaxLogFiles() const noexcept { return maxLogFiles_; }

bool LogConfig::isInitialized() const noexcept { return initialized_; }

}  // namespace spectra::vision::logger
