# Logging Framework — Quick Reference Guide

## Quick Start (5 Minutes)

### Step 1: Configure Logging at Startup

```cpp
#include "logger/LogConfig.h"
#include "logger/LoggerFactory.h"

auto config = std::make_shared<LogConfig>();
config->setConsoleOutput(true)
    ->setLogLevel(LogLevel::INFO)
    ->initialize();
```

### Step 2: Create Logger Factory

```cpp
LoggerFactory factory(config);
```

### Step 3: Inject Loggers Into Services

```cpp
class DetectionService {
public:
    explicit DetectionService(Logger logger)
        : logger_(std::move(logger)) {}

private:
    Logger logger_;
};

auto detection = std::make_unique<DetectionService>(
    factory.create("DetectionService"));
```

### Step 4: Log Messages

```cpp
logger_.info("Loading model {}", modelName);
logger_.error("Failed: {}", errorMsg);
logger_.warn("Low confidence: {:.2f}", score);
```

---

## API Reference

### LogLevel Enum

```cpp
enum class LogLevel : int {
    TRACE = 0,    // Detailed flow, variables
    DEBUG = 1,    // Method params, decisions
    INFO = 2,     // General info (DEFAULT)
    WARN = 3,     // Warnings, recoverable errors
    ERROR = 4,    // Failed operations
    FATAL = 5,    // Critical failures
    OFF = 6,      // Disable all logging
};
```

### Logger Methods

```cpp
Logger logger("ComponentName");

// Log at different levels
logger.trace("message {}", arg);    // Most verbose (often disabled)
logger.debug("message {}", arg);    // Debug info (often disabled)
logger.info("message {}", arg);     // General info (default enabled)
logger.warn("message {}", arg);     // Warnings (default enabled)
logger.error("message {}", arg);    // Errors (default enabled)
logger.fatal("message {}", arg);    // Critical (always logged)

// Query methods
std::string_view name = logger.getComponent();
bool enabled = Logger::isEnabled(LogLevel::DEBUG);
```

### LogConfig Methods

```cpp
auto config = std::make_shared<LogConfig>();

// Builder pattern (chainable)
config->setConsoleOutput(bool)           // Enable/disable console
    ->setFileOutput(path)                // File path ("" to disable)
    ->setLogLevel(LogLevel)              // Min level to log
    ->setLogRotation(bool)               // Enable/disable rotation
    ->setLogFileSizeLimit(bytes)         // Rotation size (default 10MB)
    ->setMaxLogFiles(count)              // Backup files (default 5)
    ->initialize();                      // Apply configuration

// Query methods (after init)
bool console = config->isConsoleOutputEnabled();
std::string file = config->getFileOutputPath();
LogLevel level = config->getLogLevel();
bool rotation = config->isLogRotationEnabled();
size_t size = config->getLogFileSizeLimit();
int count = config->getMaxLogFiles();
bool init = config->isInitialized();
```

### LoggerFactory Methods

```cpp
LoggerFactory factory(config);

Logger logger = factory.create("ComponentName");
const LogConfig& cfg = factory.getConfig();
```

---

## Usage Patterns

### Simple Application

```cpp
#include "logger/LogConfig.h"
#include "logger/LoggerFactory.h"

int main() {
    // 1. Configure
    auto config = std::make_shared<LogConfig>();
    config->setConsoleOutput(true)
        ->setLogLevel(LogLevel::INFO)
        ->initialize();

    // 2. Create factory
    LoggerFactory factory(config);

    // 3. Create logger
    auto logger = factory.create("MyApp");

    // 4. Use logger
    logger.info("Application started");
    logger.warn("This is a warning");
    logger.error("Error occurred: {}", errorMsg);

    return 0;
}
```

### Service with Dependency Injection

```cpp
class MyService {
public:
    MyService(Logger logger, std::shared_ptr<IComponent> component)
        : logger_(std::move(logger)),
          component_(std::move(component)) {}

    void doWork() {
        logger_.info("Starting work");
        try {
            component_->process();
            logger_.info("Work completed");
        } catch (const std::exception& e) {
            logger_.error("Work failed: {}", e.what());
        }
    }

private:
    Logger logger_;
    std::shared_ptr<IComponent> component_;
};

// Usage
auto config = std::make_shared<LogConfig>();
config->setConsoleOutput(true)->initialize();

LoggerFactory factory(config);

auto service = std::make_shared<MyService>(
    factory.create("MyService"),
    createComponent());

service->doWork();
```

### Multiple Services

```cpp
// Create all loggers in one place
auto detectionLogger = factory.create("DetectionService");
auto modelLogger = factory.create("ModelService");
auto preprocessLogger = factory.create("PreprocessingService");

// Inject into services
auto detection = std::make_shared<DetectionService>(
    std::move(detectionLogger));
auto models = std::make_shared<ModelService>(
    std::move(modelLogger));
auto preprocess = std::make_shared<PreprocessingService>(
    std::move(preprocessLogger));
```

### Format String Examples

```cpp
Logger logger("Component");

// String
logger.info("Processing file: {}", filename);

// Integer
logger.debug("Frame count: {}", count);

// Floating point
logger.warn("Confidence: {:.2f}%", confidence * 100);

// Multiple args
logger.info("Frame {}: {} detections in {:.3f}s",
            frameId, detectionCount, elapsed);

// Mixed types
logger.error("Failed to load {} from {} after {} retries: {}",
             modelName, modelPath, retries, errorMsg);
```

---

## Configuration Examples

### Development (Verbose)

```cpp
auto config = std::make_shared<LogConfig>();
config->setConsoleOutput(true)
    ->setFileOutput("./logs/dev.log")
    ->setLogLevel(LogLevel::DEBUG)  // Include debug messages
    ->initialize();
```

### Production (Minimal)

```cpp
auto config = std::make_shared<LogConfig>();
config->setConsoleOutput(false)      // No console spam
    ->setFileOutput("./logs/app.log")
    ->setLogLevel(LogLevel::WARN)    // Only warnings and errors
    ->setLogRotation(true)
    ->setLogFileSizeLimit(100 * 1024 * 1024)  // 100 MB
    ->setMaxLogFiles(10)
    ->initialize();
```

### Testing (Minimal)

```cpp
auto config = std::make_shared<LogConfig>();
config->setConsoleOutput(true)
    ->setLogLevel(LogLevel::ERROR)   // Only errors
    ->initialize();
```

### Debug Session

```cpp
auto config = std::make_shared<LogConfig>();
config->setConsoleOutput(true)
    ->setLogLevel(LogLevel::TRACE)   // All messages
    ->initialize();
```

---

## Thread-Safe Usage

### Multiple Threads Logging

```cpp
auto config = std::make_shared<LogConfig>();
config->setConsoleOutput(true)->initialize();

LoggerFactory factory(config);

std::vector<std::thread> threads;
for (int i = 0; i < 10; ++i) {
    threads.emplace_back([&factory, i]() {
        auto logger = factory.create("Worker-" + std::to_string(i));
        logger.info("Thread started");
        
        for (int j = 0; j < 10; ++j) {
            logger.debug("Processing batch {}", j);
        }
        
        logger.info("Thread finished");
    });
}

for (auto& t : threads) {
    t.join();
}
```

### Logger Ownership

```cpp
// ✅ Correct: Logger moves to thread
std::thread t([logger = std::move(logger)]() {
    logger.info("From thread");
});

// ✅ Correct: Factory shared across threads
auto factory = std::make_shared<LoggerFactory>(config);
std::thread t([factory]() {
    auto logger = factory->create("Thread");
    logger.info("From thread");
});
```

---

## Common Patterns

### Application Lifecycle Logging

```cpp
int main() {
    auto config = std::make_shared<LogConfig>();
    config->setConsoleOutput(true)->setLogLevel(LogLevel::INFO)->initialize();

    LoggerFactory factory(config);
    auto mainLogger = factory.create("Main");

    mainLogger.info("Application starting");

    try {
        // Application code
        mainLogger.info("Application running");
    } catch (const std::exception& e) {
        mainLogger.fatal("Unhandled exception: {}", e.what());
        return 1;
    }

    mainLogger.info("Application shutting down");
    return 0;
}
```

### Error Handling with Logging

```cpp
void processFrame(Logger& logger) {
    try {
        logger.debug("Processing frame");
        // Do work
        logger.info("Frame processed");
    } catch (const ValidationException& e) {
        logger.warn("Validation failed: {}", e.what());
        // Handle gracefully
    } catch (const std::exception& e) {
        logger.error("Unexpected error: {}", e.what());
        throw;
    }
}
```

### Performance-Critical Section

```cpp
void hotPath(Logger& logger) {
    // Debug logging is free when disabled
    logger.debug("Entry: param1={}, param2={}", param1, param2);
    
    // Hot loop - logger.trace() has zero cost when TRACE disabled
    for (int i = 0; i < 1000000; ++i) {
        logger.trace("Iteration {}", i);  // ← ~1 CPU cycle if disabled
        // Do work
    }
    
    logger.debug("Exit: result={}", result);
}
```

### Resource Initialization

```cpp
class ResourceManager {
public:
    ResourceManager(Logger logger) : logger_(std::move(logger)) {
        logger_.info("Initializing resources");
    }

    ~ResourceManager() {
        logger_.info("Cleaning up resources");
    }

private:
    Logger logger_;
};
```

---

## Log Output Format

```
[2026-07-24 18:45:11.237] [INFO] [DetectionService] Loading model emotion_v1.onnx
[2026-07-24 18:45:11.892] [DEBUG] [DetectionService] Model loaded in 655ms
[2026-07-24 18:45:12.134] [WARN] [PreprocessingService] Skipped 3 frames (low quality)
[2026-07-24 18:45:12.567] [ERROR] [ModelService] Failed to load model: file not found
```

**Format Explanation:**
- `[2026-07-24 18:45:11.237]` — ISO 8601 timestamp with milliseconds
- `[INFO]` — Log level
- `[DetectionService]` — Component name
- `Loading model emotion_v1.onnx` — Message with formatted arguments

---

## Filtering Logs by Component

Once logs are written to a file, you can filter by component:

```bash
# All messages from DetectionService
grep "[DetectionService]" app.log

# All errors
grep "[ERROR]" app.log

# All messages from DetectionService that are errors
grep "[DetectionService]" app.log | grep "[ERROR]"

# Real-time monitoring
tail -f app.log | grep "[ERROR]"

# Count messages by component
grep -o "\[.*\]" app.log | sort | uniq -c | sort -rn
```

---

## Troubleshooting

### Logger Not Logging Anything

**Problem:** No messages appear in logs

**Solution:**
1. Check log level: `config->getLogLevel()`
2. Verify output enabled: `config->isConsoleOutputEnabled()` or `getFileOutputPath()`
3. Confirm `.initialize()` called: `config->isInitialized()`

```cpp
// Debug: Print config
std::cout << "Log level: " << logLevelToString(config->getLogLevel()) << std::endl;
std::cout << "Console: " << config->isConsoleOutputEnabled() << std::endl;
std::cout << "File: " << config->getFileOutputPath() << std::endl;
std::cout << "Initialized: " << config->isInitialized() << std::endl;
```

### Too Many Debug Messages

**Problem:** Output is too verbose

**Solution:** Increase log level

```cpp
config->setLogLevel(LogLevel::INFO);  // Hide DEBUG/TRACE
```

### File Not Being Written

**Problem:** `setFileOutput()` specified but no file created

**Solution:**
1. Check directory exists
2. Check file permissions
3. Verify path is correct

```cpp
// Create directory if needed
namespace fs = std::filesystem;
fs::create_directories("./logs");

config->setFileOutput("./logs/app.log");
```

### Performance Issues

**Problem:** Logging is slow

**Solution:** Disable debug/trace levels

```cpp
config->setLogLevel(LogLevel::WARN);  // Disable INFO, DEBUG, TRACE
```

---

## Migration from Other Logging Systems

### From printf-style logging

```cpp
// Before
printf("Value: %d\n", value);

// After
logger.info("Value: {}", value);
```

### From cout/cerr

```cpp
// Before
std::cout << "Message: " << msg << std::endl;
std::cerr << "Error: " << err << std::endl;

// After
logger.info("Message: {}", msg);
logger.error("Error: {}", err);
```

### From global singleton

```cpp
// Before
Logger::getInstance().info("message");

// After
Logger logger("Component");
logger.info("message");
```

---

## Best Practices

1. **Create logger in constructor**
   ```cpp
   MyService(Logger logger) : logger_(std::move(logger)) {}
   ```

2. **Use component names consistently**
   ```cpp
   factory.create("DetectionService");
   factory.create("ModelService");
   factory.create("PreprocessingService");
   ```

3. **Log with context**
   ```cpp
   logger.info("Processing frame {}", frameId);  // ✓ Has context
   logger.info("Processing");                     // ✗ No context
   ```

4. **Use appropriate levels**
   ```cpp
   logger.debug("Debug details");      // ✓ Appropriate level
   logger.info("Debug details");       // ✗ Wrong level
   ```

5. **Avoid string concatenation**
   ```cpp
   logger.info("Value: {}", value);    // ✓ Use format
   logger.info("Value: " + to_string(value));  // ✗ Expensive
   ```

6. **One logger per service**
   ```cpp
   // ✓ Good: Component name identifies service
   auto logger = factory.create("DetectionService");
   ```

---

## Support

- **Architecture:** See `LOGGER_DESIGN.md`
- **Tests:** See `backend/tests/LoggerTest.cpp`
- **Examples:** See `backend/examples/LoggerExample.cpp`
