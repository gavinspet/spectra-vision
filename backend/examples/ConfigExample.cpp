#include <iostream>
#include <memory>
#include <vector>

#include "config/AppConfig.h"

// ═════════════════════════════════════════════════════════════════════════════
// EXAMPLE APPLICATION — Configuration System Usage
// ═════════════════════════════════════════════════════════════════════════════

namespace spectra::vision::example {

using config::AppConfig;
using config::ModelRegistry;

/// Example service that uses injected configuration
class ModelAnalyzerService {
public:
    /// Constructor with dependency injection
    explicit ModelAnalyzerService(
        std::shared_ptr<const AppConfig> config)
        : config_(std::move(config)) {
        if (!config_) {
            throw std::invalid_argument("Config cannot be null");
        }
    }

    /// Analyzes all registered models
    void analyzeModels() {
        const auto& models = config_->models();

        std::cout << "\n=== Model Analysis ===\n" << std::endl;
        std::cout << "Total models registered: " << models.modelCount()
                  << std::endl;

        for (const auto& model : models.allModels()) {
            analyzeModel(model);
        }
    }

    /// Looks up a specific model by ID
    void lookupModel(const std::string& modelId) {
        const auto& models = config_->models();

        if (auto result = models.getModel(modelId)) {
            const auto& model = result->get();
            std::cout << "\n=== Model Found: " << modelId << " ===\n"
                      << std::endl;
            std::cout << "  Type: " << model.modelType << std::endl;
            std::cout << "  Path: " << model.modelPath << std::endl;
            std::cout << "  Strategy: " << model.detectionStrategy
                      << std::endl;
            std::cout << "  Confidence Threshold: "
                      << model.confidenceThreshold << std::endl;
            std::cout << "  NMS Threshold: " << model.nmsThreshold
                      << std::endl;
            std::cout << "  Input Shape: [" << model.inputShape.batch << ", "
                      << model.inputShape.channels << ", "
                      << model.inputShape.height << ", "
                      << model.inputShape.width << "]" << std::endl;
            std::cout << "  Classes: ";
            for (const auto& label : model.classLabels) {
                std::cout << label << " ";
            }
            std::cout << std::endl;
        } else {
            std::cout << "Model not found: " << modelId << std::endl;
        }
    }

    /// Prints configuration summary
    void printConfigurationSummary() {
        const auto& server = config_->server();
        const auto& security = config_->security();
        const auto& preprocessing = config_->preprocessing();

        std::cout << "\n=== Configuration Summary ===\n" << std::endl;

        std::cout << "Server:" << std::endl;
        std::cout << "  Host: " << server.host << std::endl;
        std::cout << "  Port: " << server.port << std::endl;
        std::cout << "  Threads: " << server.threadCount << std::endl;
        std::cout << "  Log Level: " << server.logLevel << std::endl;

        std::cout << "\nSecurity:" << std::endl;
        std::cout << "  Auth Enabled: " << (security.enableAuth ? "yes" : "no")
                  << std::endl;
        std::cout << "  Rate Limit: " << security.rateLimitRps << " RPS"
                  << std::endl;

        std::cout << "\nPreprocessing:" << std::endl;
        std::cout << "  Target Size: " << preprocessing.targetWidth << "x"
                  << preprocessing.targetHeight << std::endl;
        std::cout << "  Normalize Scale: " << preprocessing.normalizeScale
                  << std::endl;
    }

private:
    std::shared_ptr<const AppConfig> config_;

    void analyzeModel(const config::ModelConfig& model) {
        std::cout << "\nModel: " << model.modelId << std::endl;
        std::cout << "  Type: " << model.modelType << std::endl;
        std::cout << "  Strategy: " << model.detectionStrategy << std::endl;
        std::cout << "  Classes: " << model.classLabels.size() << std::endl;
        std::cout << "  Input Shape: [" << model.inputShape.batch << ", "
                  << model.inputShape.channels << ", "
                  << model.inputShape.height << ", "
                  << model.inputShape.width << "]" << std::endl;
        std::cout << "  Thresholds: confidence=" << model.confidenceThreshold
                  << ", nms=" << model.nmsThreshold << std::endl;
    }
};

/// Example factory pattern with configuration injection
class ServiceFactory {
public:
    explicit ServiceFactory(std::shared_ptr<const AppConfig> config)
        : config_(std::move(config)) {}

    std::shared_ptr<ModelAnalyzerService> createAnalyzer() {
        return std::make_shared<ModelAnalyzerService>(config_);
    }

private:
    std::shared_ptr<const AppConfig> config_;
};

}  // namespace spectra::vision::example

// ═════════════════════════════════════════════════════════════════════════════
// MAIN EXAMPLE
// ═════════════════════════════════════════════════════════════════════════════

int main() {
    try {
        std::cout << "\n╔════════════════════════════════════════════╗"
                  << "\n║  Configuration System Example Application  ║"
                  << "\n╚════════════════════════════════════════════╝"
                  << std::endl;

        // ─────────────────────────────────────────────────────────────────────
        // STEP 1: Load configuration at startup
        // ─────────────────────────────────────────────────────────────────────
        std::cout << "\n[1] Loading configuration from 'config.json.example'..."
                  << std::endl;

        auto config = AppConfig::load("config.json.example");

        std::cout << "✓ Configuration loaded successfully" << std::endl;

        // ─────────────────────────────────────────────────────────────────────
        // STEP 2: Use factory pattern to create services
        // ─────────────────────────────────────────────────────────────────────
        std::cout << "\n[2] Creating services via factory..." << std::endl;

        using namespace spectra::vision::example;
        ServiceFactory factory(config);
        auto analyzer = factory.createAnalyzer();

        std::cout << "✓ Services created" << std::endl;

        // ─────────────────────────────────────────────────────────────────────
        // STEP 3: Print configuration summary
        // ─────────────────────────────────────────────────────────────────────
        analyzer->printConfigurationSummary();

        // ─────────────────────────────────────────────────────────────────────
        // STEP 4: Analyze all registered models
        // ─────────────────────────────────────────────────────────────────────
        analyzer->analyzeModels();

        // ─────────────────────────────────────────────────────────────────────
        // STEP 5: Direct configuration access
        // ─────────────────────────────────────────────────────────────────────
        std::cout << "\n[3] Direct configuration access..." << std::endl;

        const auto& models = config->models();
        std::cout << "Available model IDs:" << std::endl;
        for (const auto& id : models.listModelIds()) {
            std::cout << "  - " << id << std::endl;
        }

        // ─────────────────────────────────────────────────────────────────────
        // STEP 6: Look up specific models
        // ─────────────────────────────────────────────────────────────────────
        std::cout << "\n[4] Looking up specific models..." << std::endl;

        if (!models.listModelIds().empty()) {
            analyzer->lookupModel(models.listModelIds()[0]);

            if (models.modelCount() > 1) {
                analyzer->lookupModel(models.listModelIds()[1]);
            }
        }

        // ─────────────────────────────────────────────────────────────────────
        // STEP 7: Demonstrate shared ownership via multiple services
        // ─────────────────────────────────────────────────────────────────────
        std::cout << "\n[5] Demonstrating shared ownership..." << std::endl;

        auto analyzer2 = factory.createAnalyzer();

        std::cout << "  Config reference count: " << config.use_count()
                  << " (main + analyzer + analyzer2)" << std::endl;
        std::cout << "  ✓ Both services share the same immutable config"
                  << std::endl;

        // ─────────────────────────────────────────────────────────────────────
        // STEP 8: Verify immutability
        // ─────────────────────────────────────────────────────────────────────
        std::cout << "\n[6] Verifying immutability..." << std::endl;

        const auto& server = config->server();
        // This would not compile:
        // server.port = 9000;  // Error: assignment to const

        std::cout << "  ✓ Configuration is immutable (compile-time verified)"
                  << std::endl;

        std::cout << "\n╔════════════════════════════════════════════╗"
                  << "\n║              Example Completed             ║"
                  << "\n╚════════════════════════════════════════════╝\n"
                  << std::endl;

        return 0;

    } catch (const std::runtime_error& e) {
        std::cerr << "\n✗ Runtime error: " << e.what() << std::endl;
        return 1;
    } catch (const std::invalid_argument& e) {
        std::cerr << "\n✗ Validation error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Unexpected error: " << e.what() << std::endl;
        return 1;
    }
}
