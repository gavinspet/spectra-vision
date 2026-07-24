# Configuration System Design

## Overview

The Spectra Vision configuration system provides production-ready configuration management with the following key properties:

✅ **Load once during startup** — Single initialization point
✅ **Immutable after startup** — No runtime modification
✅ **Thread-safe** — Safe concurrent read access
✅ **No singleton antipattern** — Injected via dependency injection
✅ **Modern C++17** — Uses move semantics, shared_ptr, value types
✅ **Strong validation** — JSON parsing with comprehensive error checking

---

## Architecture

### Key Design Principles

1. **Value Semantics for Configuration**
   - Configuration data types (ServerConfig, ModelConfig, etc.) are simple value types
   - Copiable and movable for easy passing around
   - No mutable state or virtual functions
   - Enables efficient inline storage

2. **Immutability After Loading**
   - AppConfig holds all configuration as `const` members
   - All accessor methods return `const` references
   - No setter methods exist — configuration is sealed at construction

3. **Dependency Injection Pattern**
   - AppConfig is loaded via `AppConfig::load(path)` returning `shared_ptr<const AppConfig>`
   - This shared ownership is passed to services that need configuration
   - Services receive `std::shared_ptr<const AppConfig>` in constructor
   - No global state or getInstance() antipattern

4. **Value Types Over References**
   - Configuration structs (ServerConfig, SecurityConfig, etc.) are value types
   - Returned by value from AppConfig accessors for copy-free access
   - Small enough to stack-allocate (~50-100 bytes each)

5. **Move Semantics for Efficiency**
   - JSON parsing uses move constructors to transfer ownership
   - ModelRegistry and AppConfig use move constructors
   - Zero-copy construction from parsed JSON

---

## Components

### 1. Configuration Value Types (ConfigTypes.h)

```cpp
struct ServerConfig {
    std::string host;           // Server bind address
    std::uint16_t port;         // TCP port
    int threadCount;            // Worker threads
    std::string logPath;        // Log directory
    std::string logLevel;       // Log verbosity
};

struct SecurityConfig {
    bool enableAuth;            // Authentication enabled
    int rateLimitRps;           // Rate limit (requests/second)
    std::string jwtSecret;      // JWT signing key
};

struct PreprocessingConfig {
    int targetWidth;            // Resize width
    int targetHeight;           // Resize height
    float normalizeScale;       // Normalization factor
    float meanR, meanG, meanB;  // Channel means
};

struct InputShape {
    int batch, channels, height, width;
};

struct ModelConfig {
    std::string modelId;
    std::string modelPath;
    std::string modelType;              // "onnx", "opencv-dnn"
    std::string detectionStrategy;      // "emotion-classification", "yolo", "ssd"
    std::vector<std::string> classLabels;
    InputShape inputShape;
    float confidenceThreshold;
    float nmsThreshold;
};
```

**Properties:**
- All value types (no pointers, no virtual methods)
- Copyable and movable
- No mutable state
- `operator==` for testing and validation

---

### 2. Model Registry (ModelRegistry.h/cpp)

**Thread-safe immutable registry of model configurations.**

```cpp
class ModelRegistry {
public:
    explicit ModelRegistry(std::vector<ModelConfig> models);

    // Read operations — thread-safe, const
    std::optional<std::reference_wrapper<const ModelConfig>>
    getModel(const std::string& modelId) const;

    bool hasModel(const std::string& modelId) const;
    std::vector<std::string> listModelIds() const;
    const std::vector<ModelConfig>& allModels() const;
    std::size_t modelCount() const;

    // No mutations — registry is immutable
    // No copy — registry should not be duplicated

private:
    std::vector<ModelConfig> models_;
    void validate() const;
};
```

**Key Features:**
- Linear search with O(n) worst-case (suitable for ~10-100 models)
- Optional return type for safe lookups (no exceptions in getModel)
- Reference wrapper prevents copying and ensures shared ownership
- Validation ensures no duplicate IDs, no empty fields
- Copy is deleted — use move or share via shared_ptr

---

### 3. Main Configuration (AppConfig.h/cpp)

**Central immutable configuration holder (not a singleton).**

```cpp
class AppConfig {
public:
    // ONLY way to construct AppConfig
    static std::shared_ptr<const AppConfig> load(
        const std::string& configFilePath);

    // Immutable accessors — all const, all noexcept
    const ServerConfig& server() const noexcept;
    const SecurityConfig& security() const noexcept;
    const PreprocessingConfig& preprocessing() const noexcept;
    const ModelRegistry& models() const noexcept;

    // No setters — sealed after construction
    // No copy/move — only share via shared_ptr

private:
    explicit AppConfig(ServerConfig, SecurityConfig,
                       PreprocessingConfig, ModelRegistry);

    ServerConfig server_;
    SecurityConfig security_;
    PreprocessingConfig preprocessing_;
    ModelRegistry models_;
};
```

**Key Features:**
- Static `load()` method — entry point for loading configuration
- Private constructor — prevents direct instantiation
- Immutable after construction — all members are `const`
- Move-only semantics — cannot be copied
- Always wrapped in `shared_ptr<const AppConfig>` for injection

---

### 4. JSON Parsing (JsonParser.h/cpp)

**Internal utility functions for JSON loading and validation.**

```cpp
namespace detail {
    Json::Value loadJsonFile(const std::string& filePath);
    ServerConfig parseServerConfig(const Json::Value& root);
    SecurityConfig parseSecurityConfig(const Json::Value& root);
    PreprocessingConfig parsePreprocessingConfig(const Json::Value& root);
    ModelConfig parseModelConfig(const Json::Value& modelJson);
    InputShape parseInputShape(const Json::Value& shapeJson);
    ModelRegistry parseModelRegistry(const Json::Value& root);
}
```

**Error Handling:**
- `std::runtime_error` — File I/O failures, JSON parsing errors
- `std::invalid_argument` — Missing/invalid fields, validation failures
- Clear error messages for debugging

---

## Usage Pattern

### Application Startup

```cpp
#include "config/AppConfig.h"

int main() {
    try {
        // Load configuration once at startup
        auto config = AppConfig::load("config.json");

        // Inject into services
        auto detectionService = std::make_shared<DetectionService>(
            onnxDetector,
            preprocessor,
            config                  // Shared ownership
        );

        // Start server with config
        auto server = std::make_shared<HttpServer>(config);

        // Both services share the same config instance
        server->start();

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Startup failed: " << e.what() << std::endl;
        return 1;
    }
}
```

### Service Constructor

```cpp
class DetectionService {
public:
    DetectionService(
        std::shared_ptr<IDetector> detector,
        std::shared_ptr<IPreprocessingService> preprocessor,
        std::shared_ptr<const AppConfig> config)  // Injected
        : detector_(std::move(detector)),
          preprocessor_(std::move(preprocessor)),
          config_(std::move(config)) {}

    void processFrame(const Frame& frame) {
        // Access configuration as const
        float threshold = config_->models()
            .getModel("emotion-v1")
            ->get()
            .confidenceThreshold;

        // Use configuration
        auto result = detector_->detect(frame, threshold);
    }

private:
    std::shared_ptr<IDetector> detector_;
    std::shared_ptr<IPreprocessingService> preprocessor_;
    std::shared_ptr<const AppConfig> config_;
};
```

### Reading Configuration

```cpp
// Access via const references (no copying)
const auto& serverCfg = config->server();
const auto& securityCfg = config->security();
const auto& modelRegistry = config->models();

// Safe lookups
if (auto model = modelRegistry.getModel("emotion-v1")) {
    float threshold = model->get().confidenceThreshold;
}

// List models
for (const auto& modelId : modelRegistry.listModelIds()) {
    std::cout << "Model: " << modelId << std::endl;
}
```

---

## Configuration File Format (config.json)

```json
{
  "server": {
    "host": "0.0.0.0",
    "port": 8080,
    "threadCount": 16,
    "logPath": "./logs",
    "logLevel": "info"
  },
  "security": {
    "enableAuth": true,
    "rateLimitRps": 100,
    "jwtSecret": "production-secret-key"
  },
  "preprocessing": {
    "targetWidth": 224,
    "targetHeight": 224,
    "normalizeScale": 0.00392156862745098,
    "meanR": 0.485,
    "meanG": 0.456,
    "meanB": 0.406
  },
  "models": [
    {
      "modelId": "emotion-v1",
      "modelPath": "models/emotion_recognition.onnx",
      "modelType": "onnx",
      "detectionStrategy": "emotion-classification",
      "classLabels": ["angry", "disgust", "fear", "happy", "neutral", "sad", "surprise"],
      "inputShape": {
        "batch": 1,
        "channels": 3,
        "height": 224,
        "width": 224
      },
      "confidenceThreshold": 0.6,
      "nmsThreshold": 0.45
    }
  ]
}
```

---

## Thread Safety

### Safe Concurrent Access

1. **Multiple threads reading config** — Safe ✅
   - `const AppConfig` object is immutable
   - `shared_ptr` reference counting is atomic (thread-safe)
   - All methods are `const` and `noexcept`

2. **Thread-local model lookups** — Safe ✅
   ```cpp
   std::thread t1([config]() {
       auto model = config->models().getModel("emotion-v1");
       // Multiple threads can do this concurrently
   });
   ```

3. **Reading from different threads** — Safe ✅
   - No shared mutable state
   - Each thread accesses read-only const references
   - No locks required

### NOT Thread-Safe

1. **Modifying configuration at runtime** — Not allowed
   - AppConfig is immutable by design
   - No setter methods exist
   - Attempting to modify (via const_cast) is undefined behavior

2. **Loading configuration on multiple threads** — Not required
   - Configuration is loaded once during startup
   - Typically on the main thread
   - No synchronization needed

---

## Immutability Guarantees

### Compile-Time Guarantees

```cpp
auto config = AppConfig::load("config.json");

// ✅ This works — const access
const auto& srv = config->server();
int port = srv.port;

// ❌ This does NOT compile — trying to modify
// config->server().port = 9000;  // Compiler error!

// ❌ This does NOT compile — trying to copy AppConfig
// auto copy = *config;  // Deleted copy constructor

// ❌ This does NOT compile — trying to move AppConfig
// auto moved = std::move(*config);  // Deleted move constructor
```

### Runtime Guarantees

- Once `AppConfig::load()` completes, configuration is sealed
- No modifications possible
- No dynamic reloading
- Configuration changes require restart

---

## Error Handling

### Exceptions During Loading

```cpp
try {
    auto config = AppConfig::load("config.json");
} catch (const std::runtime_error& e) {
    // File not found or JSON parse error
    // e.what() contains details
} catch (const std::invalid_argument& e) {
    // Missing field, invalid value, or validation failure
    // e.what() contains field details
}
```

### Validation Failures

```cpp
// ModelRegistry validation
- Duplicate model IDs → std::invalid_argument
- Empty modelId → std::invalid_argument
- Empty modelPath → std::invalid_argument
- Empty classLabels → std::invalid_argument

// AppConfig validation
- Missing "server" section → std::invalid_argument
- Missing "models" section → std::invalid_argument
- Invalid port number → std::invalid_argument
- Invalid thread count → std::invalid_argument
- Invalid thresholds (not in [0,1]) → std::invalid_argument
```

---

## Best Practices

### ✅ DO

1. **Load once at startup**
   ```cpp
   auto config = AppConfig::load("config.json");
   // Store in main application object
   ```

2. **Inject via shared_ptr**
   ```cpp
   service->setConfig(config);  // Pass shared ownership
   ```

3. **Use const references in constructors**
   ```cpp
   Service(std::shared_ptr<const AppConfig> cfg)
       : config_(std::move(cfg)) {}
   ```

4. **Read-only access throughout application**
   ```cpp
   const auto& threshold = config_->models()
       .getModel("model-id")
       ->get()
       .confidenceThreshold;
   ```

### ❌ DON'T

1. **Don't create AppConfig directly**
   ```cpp
   // Wrong
   AppConfig cfg;  // Compiler error — private constructor
   ```

2. **Don't use singleton pattern**
   ```cpp
   // Wrong
   auto cfg = AppConfig::getInstance();  // No such method
   ```

3. **Don't modify configuration at runtime**
   ```cpp
   // Wrong
   auto config = AppConfig::load("config.json");
   const_cast<ServerConfig&>(config->server()).port = 9000;  // UB!
   ```

4. **Don't copy AppConfig**
   ```cpp
   // Wrong
   auto copy = *config;  // Compiler error — deleted copy constructor
   ```

5. **Don't hold raw pointers to configuration**
   ```cpp
   // Wrong
   AppConfig* ptr = config.get();
   // Lifetime issues, no shared ownership
   ```

---

## Performance Characteristics

### Memory Usage

```
ServerConfig:           ~100 bytes
SecurityConfig:         ~80 bytes  
PreprocessingConfig:    ~40 bytes
ModelConfig (1 model):  ~150-200 bytes (depending on label count)
ModelRegistry:          8 bytes (vector) + N × ModelConfig
AppConfig:              ~300-400 bytes
shared_ptr overhead:    ~16 bytes per ptr
```

**Total for typical 3-model setup:**
- Configuration data: ~600-800 bytes
- Per-service shared_ptr: ~16 bytes each

### Lookup Performance

```cpp
// Model lookup — O(n) linear search
auto model = config->models().getModel("emotion-v1");
// 10 models: ~10 comparisons
// 100 models: ~100 comparisons
// Typically < 1 microsecond for typical setups
```

### Thread Safety Overhead

- No locks required — configuration is immutable
- shared_ptr uses atomic reference counting (very efficient)
- Contention-free concurrent reads

---

## Testing

### Unit Tests Provided

See `backend/tests/ConfigTest.cpp`:

1. **ConfigTypes Tests**
   - ModelConfig equality
   - Default values for ServerConfig

2. **ModelRegistry Tests**
   - Construction with valid/invalid models
   - getModel() by ID
   - hasModel() checks
   - listModelIds() enumeration
   - Duplicate ID detection
   - Empty field validation
   - Move semantics
   - Copy prevention

3. **AppConfig Tests**
   - Loading from file
   - Immutability verification
   - Shared pointer ownership
   - Copy prevention
   - Dependency injection patterns

4. **Thread Safety Tests**
   - Concurrent model lookups
   - Concurrent config access
   - Shared pointer atomicity

### Running Tests

```bash
# Build tests
cmake --preset debug
cmake --build build/debug --parallel

# Run configuration tests
ctest --test-dir build/debug --verbose -R Config
```

---

## Migration from Singleton

If you have existing code using singleton pattern:

### Before (Singleton - Antipattern)

```cpp
class AppConfig {
public:
    static AppConfig& getInstance() {
        static AppConfig instance;
        return instance;
    }
};

// Usage
AppConfig::getInstance().server().port;  // Global state
```

### After (Dependency Injection - Pattern)

```cpp
class Service {
    std::shared_ptr<const AppConfig> config_;
public:
    Service(std::shared_ptr<const AppConfig> cfg)
        : config_(std::move(cfg)) {}

    void method() {
        config_->server().port;  // Injected dependency
    }
};

// Usage
auto config = AppConfig::load("config.json");
auto service = std::make_shared<Service>(config);
```

**Benefits:**
- Testable — inject mock config in tests
- No global state — easier to reason about
- Composable — multiple services share same config
- Thread-safe — immutable shared data

---

## Summary

| Property | Implementation |
|----------|---|
| **Load Timing** | Once at startup via `AppConfig::load()` |
| **Mutability** | Immutable after load (all members const) |
| **Thread Safety** | Safe for concurrent reads (no locks needed) |
| **Singleton Antipattern** | ✅ Avoided — use dependency injection |
| **Memory Ownership** | `shared_ptr<const AppConfig>` for injection |
| **Configuration Reload** | Not supported — restart required |
| **Validation** | Comprehensive during load (exceptions on error) |
| **Performance** | Zero-copy const access, O(n) model lookup |
| **C++ Standard** | C++17 (move semantics, structured bindings) |
