# Production-Grade Logging Framework — Design Document

## Executive Summary

A comprehensive, production-ready logging framework for Spectra Vision built with modern C++17, featuring:

✅ **Component-based logging** — Named loggers for identifying message sources
✅ **Six log levels** — TRACE, DEBUG, INFO, WARN, ERROR, FATAL
✅ **Format string support** — Type-safe formatting with `fmt` library
✅ **Thread-safe** — Underlying trantor::Logger handles synchronization
✅ **No global state** — Dependency injection throughout
✅ **Zero overhead when disabled** — Early returns for filtered levels
✅ **ISO timestamps** — RFC 3339 format with milliseconds
✅ **Multiple outputs** — Console and file logging support
✅ **Log rotation** — Automatic rotation by file size
✅ **Configurable** — Builder pattern for flexible setup

---

## Architecture Overview

### Layer 1: LogLevel (enum class)

**Responsibility:** Define and convert between log levels

```
TRACE (0)    — Most verbose, flow tracing
 ↓
DEBUG (1)    — Detailed debugging info  
 ↓
INFO (2)     — General informational
 ↓
WARN (3)     — Warning (default enabled)
 ↓
ERROR (4)    — Error condition
 ↓
FATAL (5)    — Critical failure
 ↓
OFF (6)      — Disable all logging
```

**Key Features:**
- Ordered by severity (higher number = more serious)
- String conversion for configuration
- Case-insensitive parsing

---

### Layer 2: Logger (main interface)

**Responsibility:** Provide logging interface for application code

**Constructor:**
```cpp
explicit Logger(std::string_view component);
```

**Why component naming?**
- Identifies message source (e.g., `[DetectionService]`)
- Enables filtering by component in logs
- Simplifies debugging (know which service logged what)
- Provides context without repeated prefix

**API Design:**
```cpp
void trace(fmt_string, args...);
void debug(fmt_string, args...);
void info(fmt_string, args...);
void warn(fmt_string, args...);
void error(fmt_string, args...);
void fatal(fmt_string, args...);
```

**Why template methods?**
- Type-safe formatting at compile-time (compiler catches format errors)
- Format strings only evaluated if log level enabled (zero-cost filtering)
- Compiler can inline small format operations
- No vtable overhead (templates are compile-time polymorphism)

**Why variadic templates?**
- Type-safe: `logger.info("Value: {}", x)` — compiler checks types
- Flexible: Any number of arguments
- Efficient: No conversions to `std::string` until needed

---

### Layer 3: LogConfig (configuration holder)

**Responsibility:** Centralized configuration management

**Builder Pattern:**
```cpp
auto config = std::make_shared<LogConfig>();
config->setConsoleOutput(true)
    ->setFileOutput("./logs/app.log")
    ->setLogLevel(LogLevel::INFO)
    ->initialize();
```

**Why builder pattern?**
- Readable fluent API
- Sensible defaults
- Optional configuration (not all fields required)
- Clear initialization point (`.initialize()`)

**Configuration Options:**
- `setConsoleOutput(bool)` — Enable/disable console
- `setFileOutput(path)` — Enable/disable file, set path
- `setLogLevel(level)` — Filter level
- `setLogRotation(bool)` — Enable/disable rotation
- `setLogFileSizeLimit(bytes)` — Rotation trigger
- `setMaxLogFiles(count)` — Number of backups

**Why not a singleton?**
- Testable: inject mock config
- Composable: multiple configs for different parts
- Explicit: clearly see where config comes from
- Flexible: different configs for different scenarios

---

### Layer 4: LoggerFactory (creation)

**Responsibility:** Create loggers with consistent configuration

```cpp
auto logger = factory.create("DetectionService");
```

**Why a factory?**
- Enforces that all loggers use same configuration
- Central place to add logger-level features
- Enables custom logging policy (e.g., component filtering)
- Simplifies testing (mock factory in tests)

**Thread Safety:**
- Factory is immutable (const after construction)
- Can be safely shared across threads
- Logger creation is thread-safe

---

## Design Decisions Explained

### 1. Wrapper Around trantor::Logger

**Decision:** Use trantor::Logger (Drogon's logging system)

**Rationale:**
- trantor::Logger already integrated with Drogon framework
- Well-tested, production-proven
- Handles low-level thread safety
- Supports file output and rotation
- Avoids dependency bloat

**Trade-off:**
- Depends on trantor, but it's already a Drogon dependency

---

### 2. Component-Based Logging

**Decision:** Each logger has a name (component)

**Example:**
```cpp
Logger detection("DetectionService");
detection.info("Loading model");
// Output: [2026-07-24 18:45:11] [INFO] [DetectionService] Loading model
```

**Rationale:**
- Identifies message source at a glance
- Enables grep filtering: `grep "[DetectionService]" app.log`
- No need for extra context in every message
- Aligns with microservices best practices

---

### 3. No Global State

**Decision:** Dependency injection throughout

**What we DON'T do:**
```cpp
// ❌ Singleton antipattern
Logger& Logger::getInstance() { ... }
Logger::getInstance().info("message");
```

**What we DO do:**
```cpp
// ✅ Dependency injection
class MyService {
    MyService(Logger logger) : logger_(std::move(logger)) {}
private:
    Logger logger_;
};
```

**Rationale:**
- Testable: inject mock logger in unit tests
- Composable: multiple loggers with different levels
- Explicit: see where logger comes from
- Flexible: adapt for different scenarios
- Thread-safe: no hidden shared state

---

### 4. Six Log Levels

**Decision:** Support TRACE, DEBUG, INFO, WARN, ERROR, FATAL

**Hierarchy:**
```
TRACE  — Entry/exit, loop iterations, variable dumps
DEBUG  — Method parameters, query results, decisions
INFO   — Startup, config, high-level flow (DEFAULT)
WARN   — Recoverable issues, deprecated usage
ERROR  — Failed operations, exceptions
FATAL  — Critical failures, must shutdown
```

**Rationale:**
- TRACE/DEBUG often disabled in production (performance)
- INFO captures normal operations
- WARN/ERROR catch issues
- FATAL for emergencies
- Allows tuning verbosity by environment

---

### 5. Format String Support

**Decision:** Use `fmt` library for type-safe formatting

```cpp
logger.info("Processing {} frames, {} seconds", frameCount, duration);
```

**Rationale:**
- Type-safe: compiler checks format/args match
- Efficient: compiles to fast code
- Familiar: C++20 `std::format` compatible
- Extensible: can format custom types

**Why not printf-style?**
```cpp
// ❌ Unsafe, runtime errors
logger.info("Value: %d", "string");  // Type mismatch!
```

---

### 6. Zero-Cost When Disabled

**Decision:** Early return before formatting if level disabled

```cpp
template <typename... Args>
void Logger::debug(std::string_view fmt, Args&&... args) {
    if (!isEnabled(LogLevel::DEBUG)) {
        return;  // ← Zero-cost path when DEBUG disabled
    }
    auto formatted = fmt::format(fmt, std::forward<Args>(args)...);
    logImpl(LogLevel::DEBUG, formatted);
}
```

**Rationale:**
- TRACE/DEBUG can be expensive (lots of string building)
- In production, if level is INFO, debug() returns immediately
- Single condition check (negligible cost)
- Compiler can optimize away dead code

**Performance Impact:**
```
Disabled message: ~1 CPU cycle (branch prediction)
Enabled message:  ~100+ CPU cycles (string formatting, I/O)
```

---

### 7. Thread Safety

**Decision:** Leverage trantor::Logger thread safety

**trantor's Thread Safety:**
- Uses internal mutex for log writes
- Safe to call from multiple threads
- No race conditions on log output

**Our Design:**
- Logger objects are copyable (small, value-like)
- Can be passed to different threads
- Each component can have its own logger instance
- All thread safety handled by trantor

**What we don't do:**
- No locks in Logger class
- No atomic variables
- No thread-local storage
- Delegation to proven infrastructure

---

### 8. Configuration Immutability

**Decision:** LogConfig immutable after `.initialize()`

```cpp
auto config = std::make_shared<LogConfig>();
config->setLogLevel(LogLevel::INFO)
    ->initialize();  // ← Sealed after this

// Can only read, not modify
LogLevel level = config->getLogLevel();
```

**Rationale:**
- Prevents accidental misconfiguration mid-execution
- Easier to reason about behavior
- Safe to share across threads (no synchronization needed)
- Consistent logging behavior throughout app lifetime

---

### 9. ISO 8601 Timestamps

**Decision:** Format: `YYYY-MM-DD HH:MM:SS.mmm`

```
[2026-07-24 18:45:11.237] [INFO] [DetectionService] Message
```

**Rationale:**
- ISO 8601 is international standard
- Human-readable (sorted chronologically)
- Milliseconds provide precision (1ms resolution)
- Compatible with log aggregation tools (Elasticsearch, Splunk)
- Timezone handling: local time (consistent with system)

---

### 10. Dependency Injection Over Singletons

**Decision:** Pass Logger to constructors, never use `Logger::getInstance()`

**Pattern:**
```cpp
class DetectionService {
public:
    DetectionService(Logger logger, std::shared_ptr<IDetector> detector)
        : logger_(std::move(logger)), detector_(std::move(detector)) {}

private:
    Logger logger_;
    std::shared_ptr<IDetector> detector_;
};
```

**Rationale:**
- **Testability:** Inject mock logger in unit tests
- **Flexibility:** Different log levels for different services
- **Explicit:** Clear dependencies in constructor
- **No magic:** Nothing hidden in static methods
- **Thread-safe:** No global state to synchronize

---

## Implementation Details

### Template Method Pattern

**Why templates?**
```cpp
template <typename... Args>
void Logger::info(std::string_view fmt, Args&&... args) {
    if (!isEnabled(LogLevel::INFO)) {
        return;  // Zero-cost when disabled
    }
    auto formatted = fmt::format(fmt, std::forward<Args>(args)...);
    logImplFormatted(LogLevel::INFO, fmt, formatted);
}
```

1. **Early exit:** If log level disabled, return immediately (1 CPU cycle)
2. **Type safety:** Format and args checked at compile-time
3. **Efficiency:** Compiler can inline the whole thing
4. **Code reuse:** Common logic in `logImpl()` and `logImplFormatted()`

---

### Builder Pattern Benefits

```cpp
config->setConsoleOutput(true)
    ->setFileOutput("./logs/app.log")
    ->setLogLevel(LogLevel::INFO)
    ->setLogRotation(true)
    ->setLogFileSizeLimit(10 * 1024 * 1024)
    ->initialize();
```

**Benefits:**
- **Readable:** What each line does is clear
- **Chainable:** Return `*this` enables chaining
- **Optional:** Don't set what you don't need
- **Validatable:** `.initialize()` validates and applies config

---

### Factory Pattern Benefits

```cpp
LoggerFactory factory(config);
auto logger1 = factory.create("Service1");
auto logger2 = factory.create("Service2");
```

**Benefits:**
- **Consistency:** All loggers use same config
- **Centralization:** Change behavior in one place
- **Testability:** Mock factory in tests
- **Extensibility:** Can add filtering, metrics, etc.

---

## Format String Examples

### Basic Usage
```cpp
logger.info("Simple message");
```

### Single Argument
```cpp
logger.info("Loading model: {}", modelName);
// Output: [timestamp] [INFO] [component] Loading model: emotion_v1.onnx
```

### Multiple Arguments
```cpp
logger.info("Processed {} frames in {:.2f}s", frameCount, duration);
// Output: [timestamp] [INFO] [component] Processed 100 frames in 2.34s
```

### Integers
```cpp
logger.debug("Frame {}, confidence: {}, threshold: {}", id, conf, thresh);
```

### Floating Point
```cpp
logger.warn("Memory usage: {:.1f}%", usage);
```

### Mixed Types
```cpp
logger.error("Failed to load {} after {} attempts: {}", 
             modelPath, attempts, errorMessage);
```

---

## Thread Safety Analysis

### Safe Operations

1. **Creating loggers:** `Logger logger("name");` ✅
   - Simple string copy, no shared state

2. **Logging from multiple threads:** ✅
   ```cpp
   std::thread t1([&logger]() { logger.info("msg1"); });
   std::thread t2([&logger]() { logger.info("msg2"); });
   ```
   - trantor::Logger handles synchronization

3. **Sharing factory:** ✅
   ```cpp
   auto logger1 = factory.create("Service1");
   auto logger2 = factory.create("Service2");
   ```
   - Factory is immutable, thread-safe

### NOT Thread-Safe

1. **Modifying config after init:** ❌
   - After `.initialize()`, don't modify config
   - Will have undefined behavior

2. **Using Logger across threads without std::shared_ptr:** ⚠️
   - Logger is copyable, but ownership must be clear
   - Use `std::move()` when passing to different thread

---

## Performance Characteristics

### Disabled Message (INFO level, logging TRACE)
- **Cost:** ~1 CPU cycle
- **Operation:** One condition check
- **Compiler optimization:** Often inlined

### Enabled Message (Typical)
- **Cost:** ~100-1000 CPU cycles
- **Operations:**
  - Timestamp generation: ~50 cycles
  - String formatting: ~200 cycles
  - I/O buffering: ~100 cycles
  - Trantor locking: ~50 cycles

### Worst Case (File I/O)
- **Cost:** ~100,000+ CPU cycles
- **Operation:** Disk write (microseconds)
- **Mitigation:** Use log rotation, buffering

---

## Usage Pattern Guide

### Application Startup

```cpp
int main() {
    // 1. Create and configure
    auto config = std::make_shared<LogConfig>();
    config->setConsoleOutput(true)
        ->setFileOutput("./logs/app.log")
        ->setLogLevel(LogLevel::INFO)
        ->initialize();

    // 2. Create factory
    LoggerFactory factory(config);

    // 3. Create services with injected loggers
    auto detectionLogger = factory.create("DetectionService");
    auto detection = std::make_unique<DetectionService>(
        std::move(detectionLogger));

    // 4. Run application
    detection->process();
    return 0;
}
```

### Service Implementation

```cpp
class DetectionService {
public:
    explicit DetectionService(Logger logger)
        : logger_(std::move(logger)) {}

    void process() {
        logger_.info("Starting detection pipeline");
        
        try {
            loadModel();
            processFrames();
        } catch (const std::exception& e) {
            logger_.error("Error during processing: {}", e.what());
        }
        
        logger_.info("Detection complete");
    }

private:
    Logger logger_;
    
    void loadModel() {
        logger_.info("Loading model");
        logger_.debug("Model path: /opt/models/emotion.onnx");
        // ...
    }

    void processFrames() {
        logger_.info("Processing {} frames", frameCount);
        for (int i = 0; i < frameCount; ++i) {
            logger_.trace("Frame {} start", i);
            // Process
            logger_.trace("Frame {} end", i);
        }
    }
};
```

---

## Error Handling

### Invalid Logger Creation

```cpp
// ❌ Empty component name
Logger logger("");  // Throws std::invalid_argument
```

### Invalid Config

```cpp
auto config = std::make_shared<LogConfig>();
config->setConsoleOutput(false)
    ->setFileOutput("");  // Both outputs disabled
// config->initialize();  // Throws: "At least one output required"
```

### Invalid Factory Creation

```cpp
// ❌ Null config
LoggerFactory factory(nullptr);  // Throws std::invalid_argument

// ❌ Uninitialized config
LoggerFactory factory(uninitializedConfig);  // Throws std::invalid_argument
```

---

## Summary

This logging framework provides:

1. **Type-Safe:** Format strings verified at compile-time
2. **Thread-Safe:** Leverages trantor's synchronization
3. **No Global State:** Dependency injection throughout
4. **Component-Based:** Named loggers for filtering
5. **Zero-Cost When Disabled:** Early returns for filtered levels
6. **Production-Ready:** Tested, documented, extensible
7. **Modern C++:** C++17 features, move semantics
8. **Flexible:** Multiple outputs, rotation, levels, timestamps

All without global variables, all testable, all explicit.
