#pragma once

#include <memory>
#include <string>

#include "ConfigTypes.h"
#include "ModelRegistry.h"

namespace spectra::vision::config {

// ═════════════════════════════════════════════════════════════════════════════
// APP CONFIG — Immutable application configuration (loaded once at startup)
// ═════════════════════════════════════════════════════════════════════════════

/// Central application configuration holder.
/// Loaded once during startup and immutable thereafter.
/// Intended to be injected where needed via shared_ptr<const AppConfig>.
///
/// NOT a singleton — use dependency injection instead:
///   auto config = AppConfig::load("config.json");
///   service->injectConfig(config);  // Pass around as shared_ptr
class AppConfig {
public:
    /// Loads configuration from a JSON file and returns shared ownership.
    /// This is the ONLY way to construct AppConfig.
    ///
    /// @param configFilePath Path to config.json
    /// @return Shared ownership of the loaded configuration
    /// @throws std::runtime_error if file not found or JSON is invalid
    /// @throws std::invalid_argument if required fields are missing
    static std::shared_ptr<const AppConfig> load(
        const std::string& configFilePath);

    /// Deleted copy constructor — AppConfig is move-only
    AppConfig(const AppConfig&) = delete;

    /// Deleted copy assignment — AppConfig is move-only
    AppConfig& operator=(const AppConfig&) = delete;

    /// Deleted move constructor (should use shared_ptr instead)
    AppConfig(AppConfig&&) = delete;

    /// Deleted move assignment (should use shared_ptr instead)
    AppConfig& operator=(AppConfig&&) = delete;

    // ─────────────────────────────────────────────────────────────────────────
    // Const accessors (read-only references)
    // ─────────────────────────────────────────────────────────────────────────

    /// Gets server configuration.
    /// @return Const reference (safe to share, no copying needed)
    const ServerConfig& server() const noexcept;

    /// Gets security configuration.
    /// @return Const reference
    const SecurityConfig& security() const noexcept;

    /// Gets preprocessing configuration.
    /// @return Const reference
    const PreprocessingConfig& preprocessing() const noexcept;

    /// Gets model registry.
    /// @return Const reference to thread-safe model registry
    const ModelRegistry& models() const noexcept;

    ~AppConfig() = default;

private:
    // Private constructor — only AppConfig::load() can create instances
    explicit AppConfig(ServerConfig srv, SecurityConfig sec,
                       PreprocessingConfig prep, ModelRegistry mdl);

    ServerConfig server_;
    SecurityConfig security_;
    PreprocessingConfig preprocessing_;
    ModelRegistry models_;

    // For testing only — friend access
    friend class AppConfigTest;
};

}  // namespace spectra::vision::config
