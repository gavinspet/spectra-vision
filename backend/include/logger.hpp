#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace sv {

/**
 * @brief Singleton Pattern: Global logger instance
 * 
 * Ensures only one logger instance exists throughout the application
 * lifetime using thread-safe Meyer's Singleton pattern.
 */
class Logger {
public:
    enum class Level {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };

    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    // Delete copy and move operations
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    void log(Level level, const std::string& message);
    void debug(const std::string& message) { log(Level::DEBUG, message); }
    void info(const std::string& message) { log(Level::INFO, message); }
    void warning(const std::string& message) { log(Level::WARNING, message); }
    void error(const std::string& message) { log(Level::ERROR, message); }
    void critical(const std::string& message) { log(Level::CRITICAL, message); }

private:
    Logger() = default;
    
    std::string levelToString(Level level) const;
    std::string getCurrentTimestamp() const;
};

} // namespace sv
