#include "Logger.h"
#include <trantor/utils/Logger.h>

namespace spectra::vision::utils {

LogLevel Logger::currentLevel_ = LogLevel::Info;

void Logger::setLevel(LogLevel level) { currentLevel_ = level; }

void Logger::debug(const std::string& component, const std::string& message) {
    log(LogLevel::Debug, component, message);
}
void Logger::info(const std::string& component, const std::string& message) {
    log(LogLevel::Info, component, message);
}
void Logger::warn(const std::string& component, const std::string& message) {
    log(LogLevel::Warn, component, message);
}
void Logger::error(const std::string& component, const std::string& message) {
    log(LogLevel::Error, component, message);
}

void Logger::log(LogLevel level,
                 const std::string& component,
                 const std::string& message)
{
    if (level < currentLevel_) return;
    // TODO: forward to trantor::Logger with [component] prefix
    (void)component;
    (void)message;
}

} // namespace spectra::vision::utils
