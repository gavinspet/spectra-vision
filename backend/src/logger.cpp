#include "logger.hpp"

namespace sv {

void Logger::log(Level level, const std::string& message) {
    auto timestamp = getCurrentTimestamp();
    auto levelStr = levelToString(level);
    
    std::cout << "[" << timestamp << "] [" << levelStr << "] " << message << std::endl;
}

std::string Logger::levelToString(Level level) const {
    switch (level) {
        case Level::DEBUG:    return "DEBUG";
        case Level::INFO:     return "INFO";
        case Level::WARNING:  return "WARN";
        case Level::ERROR:    return "ERROR";
        case Level::CRITICAL: return "CRIT";
        default:              return "UNKNOWN";
    }
}

std::string Logger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace sv
