# Spectra Vision - Design Patterns Documentation

## Overview

This document explains the design patterns implemented in Spectra Vision and how they contribute to clean, maintainable, and extensible code.

## 1. Strategy Pattern

### Purpose
Define a family of algorithms, encapsulate each one, and make them interchangeable.

### Implementation Location
- **Interface**: `backend/include/emotion_detection.hpp`
- **Concrete Classes**:
  - `MockEmotionDetector`: Simple mock implementation for testing
  - `AdvancedEmotionDetector`: Realistic detector with variable confidence

### Code Example

```cpp
// Strategy Interface
class EmotionDetectionStrategy {
public:
    virtual ~EmotionDetectionStrategy() = default;
    virtual EmotionResult detect(const std::string& imageData) = 0;
};

// Concrete Strategy 1: Mock
class MockEmotionDetector : public EmotionDetectionStrategy {
public:
    EmotionResult detect(const std::string& imageData) override;
};

// Concrete Strategy 2: Advanced
class AdvancedEmotionDetector : public EmotionDetectionStrategy {
public:
    EmotionResult detect(const std::string& imageData) override;
};
```

### Benefits
- ✅ **Easy to extend**: Add new detection algorithms without modifying existing code
- ✅ **Runtime selection**: Choose algorithm at runtime based on model ID
- ✅ **Testability**: Easy to mock for unit testing
- ✅ **Flexibility**: Switch between strategies without affecting client code

### Usage in Project
```cpp
// API Handler doesn't care which strategy is used
auto detector = EmotionModelFactory::create(modelId);  // Could be Mock or Advanced
auto result = detector->detect(imageData);
```

---

## 2. Factory Pattern

### Purpose
Create objects without specifying their exact classes, centralizing object creation logic.

### Implementation Location
- **Factory Class**: `backend/include/model_factory.hpp`
- **Implementation**: `backend/src/model_factory.cpp`

### Code Example

```cpp
class EmotionModelFactory {
public:
    static std::shared_ptr<EmotionDetectionStrategy> 
    create(const std::string& modelId) {
        if (modelId == "emotion-v1" || modelId == "mock") {
            return std::make_shared<MockEmotionDetector>();
        }
        else if (modelId == "emotion-v2" || modelId == "advanced") {
            return std::make_shared<AdvancedEmotionDetector>();
        }
        else {
            throw ModelException("Unknown model ID: " + modelId);
        }
    }
    
    static std::vector<std::string> listAvailableModels() {
        return {"emotion-v1", "emotion-v2", "mock", "advanced"};
    }
};
```

### Benefits
- ✅ **Centralized creation**: All model instantiation in one place
- ✅ **Easy maintenance**: Adding new models requires change in one location
- ✅ **Consistent error handling**: All creation errors handled uniformly
- ✅ **Model discovery**: Can list available models

### Usage in Project
```cpp
// Client code doesn't need to know about concrete classes
auto detector = EmotionModelFactory::create(userRequest.modelId);
// Returns appropriate strategy based on modelId
```

### Real-World Scenarios
- Model 1: Simple mock for testing
- Model 2: Advanced detector with realistic timings
- Model 3 (future): Could be ONNX Runtime-based detector
- Model 4 (future): Could be GPU-accelerated detector

---

## 3. Singleton Pattern

### Purpose
Ensure a class has only one instance and provide a global point of access to it.

### Implementation Location
- **Header**: `backend/include/logger.hpp`
- **Implementation**: `backend/src/logger.cpp`

### Code Example

```cpp
class Logger {
public:
    // Meyer's Singleton - thread-safe in C++11+
    static Logger& getInstance() {
        static Logger instance;  // Created on first call, never destroyed
        return instance;
    }
    
    // Delete copy operations to prevent duplication
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;
    
    // Public interface
    void log(Level level, const std::string& message);
    void info(const std::string& message) { log(Level::INFO, message); }
    void error(const std::string& message) { log(Level::ERROR, message); }
    
private:
    Logger() = default;  // Private constructor
};
```

### Why Meyer's Singleton?

The classic implementation (static member initialization) is:
- **Thread-safe**: C++11 guarantees thread-safe initialization
- **Lazy**: Created on first use
- **Simple**: No manual synchronization needed
- **Modern**: Uses language features, not manual locks

### Benefits
- ✅ **Single instance**: Exactly one logger throughout application
- ✅ **Global access**: Available everywhere via `Logger::getInstance()`
- ✅ **Thread-safe**: C++11 static initialization
- ✅ **Lazy initialization**: Created when first needed
- ✅ **Prevents copies**: Deleted copy/move constructors

### Usage in Project
```cpp
// Anywhere in the code
auto& logger = Logger::getInstance();
logger.info("Processing request");
logger.error("Failed to detect emotion");

// Each log includes timestamp
// Output: [2026-07-24 15:22:44] [INFO] Processing request
```

### Anti-Pattern Avoidance
This is NOT a global static logger variable:
```cpp
// BAD - global variables
Logger g_logger;  // Anyone can modify it

// GOOD - singleton
auto& logger = Logger::getInstance();  // Controlled access
```

---

## 4. Dependency Injection (Service Locator Pattern)

### Purpose
Invert control: objects don't create their dependencies; they receive them.

### Implementation Location
- **Handler**: `backend/include/api_handler.hpp`
- **Implementation**: `backend/src/api_handler.cpp`

### Code Example

```cpp
class APIHandler {
public:
    // Handler receives its dependencies (in this case, implicitly through factory)
    json handleEmotionDetection(const json& jsonRequest);
    json handleHealthCheck() const;
    json handleListModels() const;
    
private:
    void validateEmotionRequest(const json& request);
    // No emotion detector member - creates it when needed via factory
};

// Implementation
json APIHandler::handleEmotionDetection(const json& jsonRequest) {
    validateEmotionRequest(jsonRequest);
    
    std::string modelId = jsonRequest["modelId"].get<std::string>();
    std::string imageBase64 = jsonRequest["imageBase64"].get<std::string>();
    
    // Dependency injected via factory
    auto detector = EmotionModelFactory::create(modelId);
    auto result = detector->detect(imageBase64);
    
    // Format and return response
    return {
        {"classLabel", result.classLabel},
        {"confidence", result.confidence},
        {"inferenceTimeMs", result.inferenceTimeMs}
    };
}
```

### Benefits
- ✅ **Loose coupling**: Handler doesn't depend on concrete detector classes
- ✅ **Testability**: Easy to mock factory for testing
- ✅ **Flexibility**: Can change detector selection logic without modifying handler
- ✅ **Single Responsibility**: Each class has one reason to change

### Comparison with Anti-Pattern

```cpp
// BAD - Tight coupling
class APIHandler {
    EmotionDetectionStrategy* detector;
public:
    APIHandler() {
        detector = new AdvancedEmotionDetector();  // Hard-coded dependency
    }
    // Can't change to MockEmotionDetector without modifying this class
};

// GOOD - Loose coupling via dependency injection
class APIHandler {
public:
    json handleEmotionDetection(const json& request) {
        auto detector = EmotionModelFactory::create(request["modelId"]);
        // Works with any detector the factory returns
    }
};
```

### Real-World Usage
```cpp
// main.cpp
httplib::Server svr;
sv::APIHandler handler;

svr.Post("/api/v1/emotion", [&handler](const httplib::Request& req, httplib::Response& res) {
    auto request = json::parse(req.body);
    auto response = handler.handleEmotionDetection(request);
    res.set_content(response.dump(), "application/json");
});
```

---

## Design Pattern Relationships

```
┌─────────────────────────────────────────────────────┐
│              APIHandler (Dependency Injection)      │
│                                                     │
│   handleEmotionDetection(request)                  │
│          ↓                                          │
│   ┌──────────────────────────────────────────┐    │
│   │   EmotionModelFactory (Factory Pattern)  │    │
│   │                                          │    │
│   │   create(modelId) → Strategy             │    │
│   └──────────────────────────────────────────┘    │
│          ↓                                          │
│   ┌──────────────────────────────────────────┐    │
│   │ EmotionDetectionStrategy (Strategy)      │    │
│   │                                          │    │
│   │ - MockEmotionDetector                    │    │
│   │ - AdvancedEmotionDetector                │    │
│   └──────────────────────────────────────────┘    │
│                                                     │
│   Logger::getInstance() (Singleton)                │
│   └─ Single global logger instance                 │
└─────────────────────────────────────────────────────┘
```

---

## SOLID Principles Alignment

### Single Responsibility Principle (SRP)
- `EmotionDetectionStrategy`: Only responsible for detecting emotions
- `Logger`: Only responsible for logging
- `EmotionModelFactory`: Only responsible for creating detectors
- `APIHandler`: Only responsible for handling API requests

### Open/Closed Principle (OCP)
- `EmotionDetectionStrategy`: Open for extension (new detectors), closed for modification
- `EmotionModelFactory`: Can add new models without changing the interface

### Liskov Substitution Principle (LSP)
- All `EmotionDetectionStrategy` implementations can be used interchangeably
- The API handler works correctly regardless of detector implementation

### Interface Segregation Principle (ISP)
- `EmotionDetectionStrategy` has minimal, focused interface (just `detect()`)
- Clients don't depend on methods they don't use

### Dependency Inversion Principle (DIP)
- `APIHandler` depends on `EmotionModelFactory` (abstract factory), not concrete detectors
- High-level modules depend on abstractions, not low-level details

---

## Extending the Architecture

### Adding a New Emotion Detector

**Step 1**: Implement the strategy interface
```cpp
class GPUEmotionDetector : public EmotionDetectionStrategy {
public:
    EmotionResult detect(const std::string& imageData) override {
        // CUDA-based emotion detection
    }
};
```

**Step 2**: Add to factory
```cpp
// In model_factory.cpp
else if (modelId == "emotion-gpu") {
    return std::make_shared<GPUEmotionDetector>();
}
```

**That's it!** The entire system works with the new detector. No other changes needed.

---

## Performance Impact

### Memory
- Strategy pointers: 8 bytes each
- Singleton instance: Minimal (one Logger)
- Factory objects: 0 (static methods only)

### CPU
- Factory creation: O(1) hash lookup
- Singleton access: O(1) direct reference
- Polymorphic calls: Single vtable lookup

### Scalability
- Supports unlimited detector strategies
- Logger handles concurrent access via atomic increment
- No global state except singleton instance

---

## Testing Strategy

### Unit Testing with Patterns

```cpp
// Easy to test with mock detector
void testEmotionAPI() {
    // Factory returns MockEmotionDetector
    auto detector = EmotionModelFactory::create("emotion-v1");
    
    // Predictable results for testing
    auto result = detector->detect("test-image");
    assert(result.classLabel == "Happy");
    assert(result.confidence == 0.94);
}

// Easy to replace logger for testing
void testLogging() {
    auto& logger = Logger::getInstance();
    logger.info("Test message");  // Works consistently
}
```

---

## Conclusion

These four design patterns work together to create a:
- **Maintainable** codebase (easy to understand relationships)
- **Extensible** system (easy to add new features)
- **Testable** architecture (easy to mock and test)
- **Professional** implementation (industry best practices)

Each pattern solves a specific problem, and together they demonstrate professional C++ design skills.
