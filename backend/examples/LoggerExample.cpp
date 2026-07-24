#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "logger/Logger.h"
#include "logger/LogConfig.h"
#include "logger/LoggerFactory.h"

using namespace spectra::vision::logger;

// ═════════════════════════════════════════════════════════════════════════════
// EXAMPLE: Detection Service
// ═════════════════════════════════════════════════════════════════════════════

class DetectionService {
public:
    /// Constructor with injected logger
    explicit DetectionService(Logger logger) : logger_(std::move(logger)) {}

    void loadModel(const std::string& modelName) {
        logger_.info("Loading model: {}", modelName);
        logger_.debug("Model path: /opt/models/{}.onnx", modelName);

        // Simulate work
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        logger_.info("Model loaded successfully: {}", modelName);
    }

    void processFrame(int frameId) {
        logger_.debug("Processing frame {}", frameId);

        // Simulate model inference
        float confidence = 0.92f;
        std::string emotion = "happy";

        if (confidence < 0.5f) {
            logger_.warn("Low confidence: {:.2f}", confidence);
        }

        logger_.info("Frame {} detected emotion: {} (confidence: {:.2f})",
                     frameId, emotion, confidence);
    }

    void shutdown() {
        logger_.info("Shutting down detection service");
        logger_.debug("Cleaning up resources");
        logger_.trace("Shutdown complete");
    }

private:
    Logger logger_;
};

// ═════════════════════════════════════════════════════════════════════════════
// EXAMPLE: Model Service
// ═════════════════════════════════════════════════════════════════════════════

class ModelService {
public:
    explicit ModelService(Logger logger) : logger_(std::move(logger)) {}

    void registerModel(const std::string& modelId, const std::string& path) {
        logger_.info("Registering model: {}", modelId);
        logger_.debug("Model path: {}", path);
        // Registration logic...
        logger_.info("Model registered: {}", modelId);
    }

    void listModels() const {
        logger_.info("Available models: emotion-v1, face-detector, object-detector");
        logger_.debug("Total models: 3");
    }

private:
    Logger logger_;
};

// ═════════════════════════════════════════════════════════════════════════════
// EXAMPLE: Preprocessing Service
// ═════════════════════════════════════════════════════════════════════════════

class PreprocessingService {
public:
    explicit PreprocessingService(Logger logger)
        : logger_(std::move(logger)) {}

    void preprocessFrame(int frameId, int width, int height) {
        logger_.debug("Preprocessing frame {}: {}x{}", frameId, width, height);

        // Simulate preprocessing
        logger_.trace("Resizing to model input shape");
        logger_.trace("Normalizing pixel values");
        logger_.trace("Computing statistics");

        logger_.debug("Preprocessing complete for frame {}", frameId);
    }

private:
    Logger logger_;
};

// ═════════════════════════════════════════════════════════════════════════════
// MAIN EXAMPLE APPLICATION
// ═════────────────────────────────────────────────────────────────────────────

int main() {
    try {
        std::cout << "\n╔════════════════════════════════════════════╗"
                  << "\n║  Spectra Vision Logging Framework Example  ║"
                  << "\n╚════════════════════════════════════════════╝\n"
                  << std::endl;

        // ─────────────────────────────────────────────────────────────────────
        // STEP 1: Configure logging
        // ─────────────────────────────────────────────────────────────────────
        std::cout << "[1] Configuring logging system..." << std::endl;

        auto config = std::make_shared<LogConfig>();
        config->setConsoleOutput(true)
            ->setLogLevel(LogLevel::INFO)  // INFO and above
            ->initialize();

        std::cout << "✓ Logging configured" << std::endl;
        std::cout << "  - Console output: " << config->isConsoleOutputEnabled()
                  << std::endl;
        std::cout << "  - Log level: " << logLevelToString(config->getLogLevel())
                  << std::endl;

        // ─────────────────────────────────────────────────────────────────────
        // STEP 2: Create logger factory
        // ─────────────────────────────────────────────────────────────────────
        std::cout << "\n[2] Creating logger factory..." << std::endl;

        LoggerFactory factory(config);

        std::cout << "✓ Logger factory created" << std::endl;

        // ─────────────────────────────────────────────────────────────────────
        // STEP 3: Create services with injected loggers
        // ─────────────────────────────────────────────────────────────────────
        std::cout << "\n[3] Creating services with dependency injection..."
                  << std::endl;

        auto detectionLogger = factory.create("DetectionService");
        auto modelLogger = factory.create("ModelService");
        auto preprocessLogger = factory.create("PreprocessingService");

        auto detection = std::make_unique<DetectionService>(
            std::move(detectionLogger));
        auto models =
            std::make_unique<ModelService>(std::move(modelLogger));
        auto preprocessing =
            std::make_unique<PreprocessingService>(std::move(preprocessLogger));

        std::cout << "✓ Services created with injected loggers" << std::endl;

        // ─────────────────────────────────────────────────────────────────────
        // STEP 4: Demonstrate logging at different levels
        // ─────────────────────────────────────────────────────────────────────
        std::cout << "\n[4] Demonstrating logging at different levels..." << std::endl;
        std::cout << "────────────────────────────────────────────────" << std::endl;

        models->registerModel("emotion-v1", "models/emotion_v1.onnx");
        std::cout << std::endl;

        detection->loadModel("emotion_recognition");
        std::cout << std::endl;

        for (int i = 0; i < 3; ++i) {
            preprocessing->preprocessFrame(i, 640, 480);
            detection->processFrame(i);
            std::cout << std::endl;
        }

        // ─────────────────────────────────────────────────────────────────────
        // STEP 5: Demonstrate component-based filtering
        // ─────────────────────────────────────────────────────────────────────
        std::cout << "[5] Component-based logging enables filtering" << std::endl;
        std::cout << "  - Messages include component name in brackets" << std::endl;
        std::cout << "  - Can filter logs by component: grep '[DetectionService]'"
                  << std::endl;

        // ─────────────────────────────────────────────────────────────────────
        // STEP 6: Thread-safe concurrent logging
        // ─────────────────────────────────────────────────────────────────────
        std::cout << "\n[6] Demonstrating thread-safe concurrent logging..."
                  << std::endl;
        std::cout << "────────────────────────────────────────────────" << std::endl;

        std::vector<std::thread> threads;
        for (int i = 0; i < 3; ++i) {
            threads.emplace_back([&factory, i]() {
                auto logger = factory.create("WorkerThread-" + std::to_string(i));
                for (int j = 0; j < 2; ++j) {
                    logger.info("Processing batch {}", j);
                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(10));
                }
                logger.info("Thread complete");
            });
        }

        for (auto& t : threads) {
            t.join();
        }

        // ─────────────────────────────────────────────────────────────────────
        // STEP 7: Demonstrate different log levels
        // ─────────────────────────────────────────────────────────────────────
        std::cout << "\n[7] Testing log level filtering..." << std::endl;
        std::cout << "  Current level: INFO (traces/debug hidden)" << std::endl;

        auto testLogger = factory.create("LevelTest");
        testLogger.trace("This TRACE message is hidden (too verbose)");
        testLogger.debug("This DEBUG message is hidden (too verbose)");
        testLogger.info("This INFO message is visible");
        testLogger.warn("This WARN message is visible");
        testLogger.error("This ERROR message is visible");

        // ─────────────────────────────────────────────────────────────────────
        // STEP 8: Cleanup
        // ─────────────────────────────────────────────────────────────────────
        std::cout << "\n[8] Shutting down..." << std::endl;

        detection->shutdown();

        std::cout << "\n╔════════════════════════════════════════════╗"
                  << "\n║        Example Completed Successfully       ║"
                  << "\n╚════════════════════════════════════════════╝\n"
                  << std::endl;

        std::cout << "\nKey Features Demonstrated:" << std::endl;
        std::cout << "  ✓ Component-based logging (component names in brackets)"
                  << std::endl;
        std::cout << "  ✓ Multiple log levels (TRACE, DEBUG, INFO, WARN, ERROR)"
                  << std::endl;
        std::cout << "  ✓ Format string support with arguments" << std::endl;
        std::cout << "  ✓ Dependency injection pattern (no global state)" << std::endl;
        std::cout << "  ✓ Thread-safe concurrent logging" << std::endl;
        std::cout << "  ✓ Log level filtering" << std::endl;
        std::cout << "  ✓ ISO 8601 timestamps with milliseconds" << std::endl;
        std::cout << std::endl;

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\n✗ Error: " << e.what() << std::endl;
        return 1;
    }
}
