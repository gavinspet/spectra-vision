#pragma once

#include <string>
#include <cstdint>

namespace spectra::vision::config {

// Infrastructure Layer — singleton that owns the parsed application config.
// All other components read settings from here; never read env vars directly.

struct ServerConfig {
    std::string host{"0.0.0.0"};
    std::uint16_t port{8080};
    int  threadCount{16};
    std::string logPath{"./logs"};
};

struct SecurityConfig {
    bool   enableAuth{true};
    int    rateLimitRps{100};
    std::string jwtSecret;
};

class AppConfig {
public:
    static void       load(const std::string& configFilePath);
    static AppConfig& instance();

    const ServerConfig&   server()   const noexcept;
    const SecurityConfig& security() const noexcept;

private:
    AppConfig() = default;

    ServerConfig   server_{};
    SecurityConfig security_{};
};

} // namespace spectra::vision::config
