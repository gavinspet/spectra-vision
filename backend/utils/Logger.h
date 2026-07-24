#pragma once

#include <string>

namespace spectra::vision::utils {

// Shared Utilities — thin wrapper around Drogon/trantor logging that adds
// a structured [component] prefix and severity levels.

enum class LogLevel { Debug, Info, Warn, Error };

class Logger {
public:
    Logger() = delete;

    static void setLevel(LogLevel level);

    static void debug(const std::string& component, const std::string& message);
    static void info (const std::string& component, const std::string& message);
    static void warn (const std::string& component, const std::string& message);
    static void error(const std::string& component, const std::string& message);

private:
    static LogLevel currentLevel_;

    static void log(LogLevel level,
                    const std::string& component,
                    const std::string& message);
};

} // namespace spectra::vision::utils
