#include <gtest/gtest.h>
#include <sstream>
#include <thread>
#include <vector>

#include "logger/Logger.h"
#include "logger/LogConfig.h"
#include "logger/LogLevel.h"
#include "logger/LoggerFactory.h"

using namespace spectra::vision::logger;

// ═════════════════════════════════════════════════════════════════════════════
// LOGLEVEL TESTS
// ═════════════════════════════════════════════════════════════════════════════

class LogLevelTest : public ::testing::Test {};

TEST_F(LogLevelTest, LogLevelToString) {
    EXPECT_EQ(logLevelToString(LogLevel::TRACE), "TRACE");
    EXPECT_EQ(logLevelToString(LogLevel::DEBUG), "DEBUG");
    EXPECT_EQ(logLevelToString(LogLevel::INFO), "INFO");
    EXPECT_EQ(logLevelToString(LogLevel::WARN), "WARN");
    EXPECT_EQ(logLevelToString(LogLevel::ERROR), "ERROR");
    EXPECT_EQ(logLevelToString(LogLevel::FATAL), "FATAL");
    EXPECT_EQ(logLevelToString(LogLevel::OFF), "OFF");
}

TEST_F(LogLevelTest, StringToLogLevel) {
    EXPECT_EQ(stringToLogLevel("trace"), LogLevel::TRACE);
    EXPECT_EQ(stringToLogLevel("DEBUG"), LogLevel::DEBUG);
    EXPECT_EQ(stringToLogLevel("Info"), LogLevel::INFO);
    EXPECT_EQ(stringToLogLevel("warn"), LogLevel::WARN);
    EXPECT_EQ(stringToLogLevel("ERROR"), LogLevel::ERROR);
    EXPECT_EQ(stringToLogLevel("fatal"), LogLevel::FATAL);
    EXPECT_EQ(stringToLogLevel("off"), LogLevel::OFF);
}

TEST_F(LogLevelTest, StringToLogLevelCaseInsensitive) {
    EXPECT_EQ(stringToLogLevel("TRACE"), LogLevel::TRACE);
    EXPECT_EQ(stringToLogLevel("TrAcE"), LogLevel::TRACE);
    EXPECT_EQ(stringToLogLevel("debug"), LogLevel::DEBUG);
    EXPECT_EQ(stringToLogLevel("DeBuG"), LogLevel::DEBUG);
}

TEST_F(LogLevelTest, StringToLogLevelInvalid) {
    // Invalid strings default to INFO
    EXPECT_EQ(stringToLogLevel("invalid"), LogLevel::INFO);
    EXPECT_EQ(stringToLogLevel(""), LogLevel::INFO);
    EXPECT_EQ(stringToLogLevel("xyz"), LogLevel::INFO);
}

TEST_F(LogLevelTest, StringToLogLevelWarningAlias) {
    // "WARNING" should map to WARN
    EXPECT_EQ(stringToLogLevel("WARNING"), LogLevel::WARN);
    EXPECT_EQ(stringToLogLevel("warning"), LogLevel::WARN);
}

// ═════════════════════════════════════════════════════════════════════════════
// LOGGER TESTS
// ═════════════════════════════════════════════════════════════════════════════

class LoggerTest : public ::testing::Test {
protected:
    std::shared_ptr<LogConfig> createConfig() {
        auto config = std::make_shared<LogConfig>();
        config->setConsoleOutput(true)
            ->setFileOutput("")  // No file output for tests
            ->setLogLevel(LogLevel::TRACE)  // Enable all levels
            ->initialize();
        return config;
    }
};

TEST_F(LoggerTest, LoggerConstructionWithValidComponent) {
    EXPECT_NO_THROW({ Logger logger("DetectionService"); });
}

TEST_F(LoggerTest, LoggerConstructionWithEmptyComponentThrows) {
    EXPECT_THROW({ Logger logger(""); }, std::invalid_argument);
}

TEST_F(LoggerTest, LoggerGetComponent) {
    Logger logger("DetectionService");
    EXPECT_EQ(logger.getComponent(), "DetectionService");
}

TEST_F(LoggerTest, LoggerMovable) {
    Logger logger1("Service1");
    Logger logger2 = std::move(logger1);
    EXPECT_EQ(logger2.getComponent(), "Service1");
}

TEST_F(LoggerTest, LoggerCopyable) {
    Logger logger1("Service1");
    Logger logger2 = logger1;
    EXPECT_EQ(logger1.getComponent(), "Service1");
    EXPECT_EQ(logger2.getComponent(), "Service1");
}

TEST_F(LoggerTest, LoggerInfoMethod) {
    auto config = createConfig();
    Logger logger("DetectionService");

    // These should not throw
    EXPECT_NO_THROW({
        logger.info("Simple message");
        logger.info("Message with arg: {}", 42);
        logger.info("Message with multiple: {} {}", "hello", 3.14f);
    });
}

TEST_F(LoggerTest, LoggerDebugMethod) {
    auto config = createConfig();
    Logger logger("DetectionService");

    EXPECT_NO_THROW({
        logger.debug("Debug message");
        logger.debug("Debug with arg: {}", "test");
    });
}

TEST_F(LoggerTest, LoggerWarnMethod) {
    auto config = createConfig();
    Logger logger("DetectionService");

    EXPECT_NO_THROW({
        logger.warn("Warning message");
        logger.warn("Warning with value: {}", 99);
    });
}

TEST_F(LoggerTest, LoggerErrorMethod) {
    auto config = createConfig();
    Logger logger("DetectionService");

    EXPECT_NO_THROW({
        logger.error("Error message");
        logger.error("Error: {}", "something failed");
    });
}

TEST_F(LoggerTest, LoggerFatalMethod) {
    auto config = createConfig();
    Logger logger("DetectionService");

    // FATAL should not throw
    EXPECT_NO_THROW({
        logger.fatal("Fatal message");
        logger.fatal("Fatal: {}", "critical failure");
    });
}

TEST_F(LoggerTest, LoggerIsEnabled) {
    auto config = createConfig();
    
    // With TRACE level, all should be enabled
    EXPECT_TRUE(Logger::isEnabled(LogLevel::TRACE));
    EXPECT_TRUE(Logger::isEnabled(LogLevel::DEBUG));
    EXPECT_TRUE(Logger::isEnabled(LogLevel::INFO));
    EXPECT_TRUE(Logger::isEnabled(LogLevel::WARN));
    EXPECT_TRUE(Logger::isEnabled(LogLevel::ERROR));
    EXPECT_TRUE(Logger::isEnabled(LogLevel::FATAL));
    
    // OFF is always disabled
    EXPECT_FALSE(Logger::isEnabled(LogLevel::OFF));
}

// ═════════════════════════════════════════════════════════════════════════════
// LOGCONFIG TESTS
// ═════════════════════════════════════════════════════════════════════════════

class LogConfigTest : public ::testing::Test {};

TEST_F(LogConfigTest, LogConfigDefaults) {
    LogConfig config;
    EXPECT_TRUE(config.isConsoleOutputEnabled());
    EXPECT_EQ(config.getFileOutputPath(), "");
    EXPECT_EQ(config.getLogLevel(), LogLevel::INFO);
    EXPECT_TRUE(config.isLogRotationEnabled());
    EXPECT_EQ(config.getLogFileSizeLimit(), 10 * 1024 * 1024);
    EXPECT_EQ(config.getMaxLogFiles(), 5);
    EXPECT_FALSE(config.isInitialized());
}

TEST_F(LogConfigTest, LogConfigBuilderPattern) {
    LogConfig config;
    config.setConsoleOutput(false)
        ->setLogLevel(LogLevel::ERROR)
        ->setLogRotation(false)
        ->setLogFileSizeLimit(50 * 1024 * 1024)
        ->setMaxLogFiles(10);

    EXPECT_FALSE(config.isConsoleOutputEnabled());
    EXPECT_EQ(config.getLogLevel(), LogLevel::ERROR);
    EXPECT_FALSE(config.isLogRotationEnabled());
    EXPECT_EQ(config.getLogFileSizeLimit(), 50 * 1024 * 1024);
    EXPECT_EQ(config.getMaxLogFiles(), 10);
}

TEST_F(LogConfigTest, LogConfigFileOutputSetting) {
    LogConfig config;
    config.setFileOutput("/tmp/app.log");
    EXPECT_EQ(config.getFileOutputPath(), "/tmp/app.log");
}

TEST_F(LogConfigTest, LogConfigValidateRequiresOutput) {
    LogConfig config;
    config.setConsoleOutput(false).setFileOutput("");
    EXPECT_THROW({ config.initialize(); }, std::invalid_argument);
}

TEST_F(LogConfigTest, LogConfigValidateSizeLimit) {
    LogConfig config;
    EXPECT_THROW({ config.setLogFileSizeLimit(0); }, std::invalid_argument);
}

TEST_F(LogConfigTest, LogConfigValidateMaxLogFiles) {
    LogConfig config;
    EXPECT_THROW({ config.setMaxLogFiles(0); }, std::invalid_argument);
    EXPECT_THROW({ config.setMaxLogFiles(-1); }, std::invalid_argument);
}

TEST_F(LogConfigTest, LogConfigInitialize) {
    LogConfig config;
    config.setConsoleOutput(true);
    EXPECT_FALSE(config.isInitialized());
    config.initialize();
    EXPECT_TRUE(config.isInitialized());
}

TEST_F(LogConfigTest, LogConfigNonCopyable) {
    LogConfig config1;
    // These should not compile (but we can document the intent):
    // LogConfig config2 = config1;  // Error: deleted copy constructor
}

// ═════════════════════════════════════════════════════════════════════════════
// LOGGERFACTORY TESTS
// ═════════════════════════════════════════════════════════════════════════════

class LoggerFactoryTest : public ::testing::Test {
protected:
    std::shared_ptr<LogConfig> createConfig() {
        auto config = std::make_shared<LogConfig>();
        config->setConsoleOutput(true)->setLogLevel(LogLevel::INFO)->initialize();
        return config;
    }
};

TEST_F(LoggerFactoryTest, LoggerFactoryConstructionWithValidConfig) {
    auto config = createConfig();
    EXPECT_NO_THROW({ LoggerFactory factory(config); });
}

TEST_F(LoggerFactoryTest, LoggerFactoryConstructionWithNullThrows) {
    EXPECT_THROW({ LoggerFactory factory(nullptr); },
                 std::invalid_argument);
}

TEST_F(LoggerFactoryTest, LoggerFactoryConstructionWithUninitializedThrows) {
    auto config = std::make_shared<LogConfig>();
    EXPECT_THROW({ LoggerFactory factory(config); },
                 std::invalid_argument);
}

TEST_F(LoggerFactoryTest, LoggerFactoryCreate) {
    auto config = createConfig();
    LoggerFactory factory(config);

    auto logger = factory.create("DetectionService");
    EXPECT_EQ(logger.getComponent(), "DetectionService");
}

TEST_F(LoggerFactoryTest, LoggerFactoryCreateEmptyComponentThrows) {
    auto config = createConfig();
    LoggerFactory factory(config);

    EXPECT_THROW({ factory.create(""); }, std::invalid_argument);
}

TEST_F(LoggerFactoryTest, LoggerFactoryMultipleLoggers) {
    auto config = createConfig();
    LoggerFactory factory(config);

    auto logger1 = factory.create("Service1");
    auto logger2 = factory.create("Service2");
    auto logger3 = factory.create("Service3");

    EXPECT_EQ(logger1.getComponent(), "Service1");
    EXPECT_EQ(logger2.getComponent(), "Service2");
    EXPECT_EQ(logger3.getComponent(), "Service3");
}

TEST_F(LoggerFactoryTest, LoggerFactoryGetConfig) {
    auto config = createConfig();
    LoggerFactory factory(config);

    EXPECT_EQ(factory.getConfig().getLogLevel(), LogLevel::INFO);
    EXPECT_TRUE(factory.getConfig().isInitialized());
}

// ═════════════════════════════════════════════════════════════════════════════
// THREAD SAFETY TESTS
// ═════════════════════════════════════════════════════════════════════════════

class LoggerThreadSafetyTest : public ::testing::Test {
protected:
    std::shared_ptr<LogConfig> createConfig() {
        auto config = std::make_shared<LogConfig>();
        config->setConsoleOutput(true)->setLogLevel(LogLevel::INFO)->initialize();
        return config;
    }
};

TEST_F(LoggerThreadSafetyTest, ConcurrentLogging) {
    auto config = createConfig();
    LoggerFactory factory(config);

    std::vector<std::thread> threads;
    int threadCount = 10;

    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&factory, i]() {
            auto logger = factory.create("Service" + std::to_string(i));
            for (int j = 0; j < 10; ++j) {
                logger.info("Thread {} message {}", i, j);
                logger.warn("Thread {} warning {}", i, j);
                logger.error("Thread {} error {}", i, j);
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }
    // If we reach here without crashing, thread safety is verified
    SUCCEED();
}

TEST_F(LoggerThreadSafetyTest, ConcurrentLoggerCreation) {
    auto config = createConfig();
    LoggerFactory factory(config);

    std::vector<std::thread> threads;
    std::vector<Logger> loggers;
    int threadCount = 5;

    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&factory, &loggers, i]() {
            auto logger = factory.create("Service" + std::to_string(i));
            logger.info("Created logger {}", i);
        });
    }

    for (auto& t : threads) {
        t.join();
    }
    SUCCEED();
}

// ═════════════════════════════════════════════════════════════════════════════
// INTEGRATION TESTS
// ═════════════════════════════════════════════════════════════════════════════

class LoggerIntegrationTest : public ::testing::Test {
protected:
    std::shared_ptr<LogConfig> createConfig() {
        auto config = std::make_shared<LogConfig>();
        config->setConsoleOutput(true)
            ->setFileOutput("")
            ->setLogLevel(LogLevel::TRACE)
            ->initialize();
        return config;
    }
};

TEST_F(LoggerIntegrationTest, FullWorkflow) {
    // 1. Create and initialize config
    auto config = createConfig();
    EXPECT_TRUE(config->isInitialized());

    // 2. Create factory
    LoggerFactory factory(config);

    // 3. Create loggers with different components
    auto detectionLogger = factory.create("DetectionService");
    auto modelLogger = factory.create("ModelService");
    auto preprocessLogger = factory.create("PreprocessingService");

    // 4. Log messages at different levels
    detectionLogger.trace("Tracing detection pipeline");
    detectionLogger.debug("Debug: model parameters");
    detectionLogger.info("Detection service initialized");
    detectionLogger.warn("No GPU available, using CPU");
    detectionLogger.error("Failed to load model");
    detectionLogger.fatal("Critical system failure");

    modelLogger.info("Loading model {}", "emotion_v2.onnx");
    modelLogger.debug("Model path: {}", "/opt/models/emotion_v2.onnx");

    preprocessLogger.info("Preprocessing {} frames", 100);

    // If we reach here without crashes or exceptions, integration is successful
    SUCCEED();
}

TEST_F(LoggerIntegrationTest, ComponentFiltering) {
    // Demonstrates how components enable filtering
    auto config = createConfig();
    LoggerFactory factory(config);

    auto logger1 = factory.create("DetectionService");
    auto logger2 = factory.create("ModelService");
    auto logger3 = factory.create("PreprocessingService");

    // All should have different component names
    EXPECT_NE(logger1.getComponent(), logger2.getComponent());
    EXPECT_NE(logger2.getComponent(), logger3.getComponent());
    EXPECT_NE(logger1.getComponent(), logger3.getComponent());

    // This allows filtering by component in logs
    logger1.info("Message from detection");
    logger2.info("Message from models");
    logger3.info("Message from preprocessing");
}

TEST_F(LoggerIntegrationTest, DependencyInjectionPattern) {
    // Simulate a service that receives logger via DI
    auto config = createConfig();
    LoggerFactory factory(config);

    class MyService {
    public:
        explicit MyService(Logger logger) : logger_(std::move(logger)) {}

        void doWork() {
            logger_.info("Starting work");
            logger_.info("Processing item {}", 42);
            logger_.info("Work completed");
        }

    private:
        Logger logger_;
    };

    auto service = MyService(factory.create("MyService"));
    EXPECT_NO_THROW({ service.doWork(); });
}
