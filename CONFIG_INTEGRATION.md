# Configuration System Integration Guide

## Integration Overview

This guide shows how to integrate the production-ready configuration system with existing Spectra Vision services.

---

## 1. Update CMakeLists.txt

Add JsonCpp dependency to `backend/CMakeLists.txt`:

```cmake
find_package(jsoncpp REQUIRED)

# In target_link_libraries, add:
target_link_libraries(${PROJECT_NAME} PRIVATE
    jsoncpp_lib
)
```

Verify jsoncpp is already in your find_package calls.

---

## 2. Main Application (main.cpp)

### Before (Singleton Pattern)

```cpp
#include "config/AppConfig.h"

int main() {
    // Accessing global singleton
    auto& config = AppConfig::getInstance();
    auto port = config.server().port;
    
    return 0;
}
```

### After (Dependency Injection)

```cpp
#include "config/AppConfig.h"
#include "services/DetectionService.h"
#include "services/ModelService.h"

int main() {
    try {
        // Load configuration once at startup
        auto config = AppConfig::load("config.json");
        
        // Create services with injected config
        auto detector = std::make_shared<OnnxDetector>();
        auto preprocessor = std::make_shared<PreprocessingService>(config);
        auto detectionService = std::make_shared<DetectionService>(
            detector, preprocessor, config);
        auto modelService = std::make_shared<ModelService>(config);
        
        // Create HTTP server with config
        auto httpServer = std::make_shared<HttpServer>(config);
        httpServer->registerController(
            std::make_shared<DetectionController>(detectionService, config));
        httpServer->registerController(
            std::make_shared<ModelController>(modelService, config));
        
        // Start application
        httpServer->listen();
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Startup failed: " << e.what() << std::endl;
        return 1;
    }
}
```

---

## 3. Services Integration

### DetectionService

**Before:**
```cpp
class DetectionService {
    // No configuration
};
```

**After:**
```cpp
#include "config/AppConfig.h"

class DetectionService {
public:
    DetectionService(
        std::shared_ptr<IDetector> detector,
        std::shared_ptr<IPreprocessingService> preprocessor,
        std::shared_ptr<const AppConfig> config)  // NEW
        : detector_(std::move(detector)),
          preprocessor_(std::move(preprocessor)),
          config_(std::move(config)) {}

    DetectionResponse detect(const DetectionRequest& request) {
        // Use config for thresholds, model selection, etc.
        if (auto model = config_->models().getModel(request.modelId)) {
            float threshold = model->get().confidenceThreshold;
            auto result = detector_->detect(request.frame, threshold);
            return result;
        }
        throw DetectionException("Model not found: " + request.modelId);
    }

private:
    std::shared_ptr<IDetector> detector_;
    std::shared_ptr<IPreprocessingService> preprocessor_;
    std::shared_ptr<const AppConfig> config_;
};
```

### ModelService

**Before:**
```cpp
class ModelService {
    // Hardcoded model registry
};
```

**After:**
```cpp
class ModelService {
public:
    explicit ModelService(std::shared_ptr<const AppConfig> config)
        : config_(std::move(config)) {}

    std::vector<ModelInfo> listModels() const {
        std::vector<ModelInfo> models;
        for (const auto& modelCfg : config_->models().allModels()) {
            models.push_back(ModelInfo{
                modelCfg.modelId,
                modelCfg.modelType,
                modelCfg.detectionStrategy,
                static_cast<int>(modelCfg.classLabels.size())
            });
        }
        return models;
    }

    bool isModelAvailable(const std::string& modelId) const {
        return config_->models().hasModel(modelId);
    }

private:
    std::shared_ptr<const AppConfig> config_;
};
```

### PreprocessingService

**Before:**
```cpp
class PreprocessingService {
    // Hardcoded normalization values
};
```

**After:**
```cpp
class PreprocessingService {
public:
    explicit PreprocessingService(std::shared_ptr<const AppConfig> config)
        : config_(std::move(config)) {}

    Frame preprocess(const Frame& raw) {
        const auto& prep = config_->preprocessing();
        
        Frame normalized = raw;
        normalized.width = prep.targetWidth;
        normalized.height = prep.targetHeight;
        
        // Apply normalization
        applyNormalization(normalized, 
                          prep.normalizeScale,
                          prep.meanR, prep.meanG, prep.meanB);
        
        return normalized;
    }

private:
    std::shared_ptr<const AppConfig> config_;
    
    void applyNormalization(Frame& frame, float scale, 
                           float meanR, float meanG, float meanB) {
        // Implementation
    }
};
```

---

## 4. Controllers Integration

### DetectionController

**Before:**
```cpp
class DetectionController : public HttpController {
public:
    void POST(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)> callback) {
        // Hardcoded thresholds and parameters
    }
};
```

**After:**
```cpp
class DetectionController : public HttpController {
public:
    DetectionController(
        std::shared_ptr<IDetectionService> service,
        std::shared_ptr<const AppConfig> config)
        : service_(std::move(service)),
          config_(std::move(config)) {}

    void POST(const HttpRequestPtr& req, 
              std::function<void(const HttpResponsePtr&)> callback) {
        try {
            auto request = parseRequest(req);
            
            // Validate model is registered
            if (!config_->models().hasModel(request.modelId)) {
                return sendError(callback, "Model not found");
            }
            
            auto response = service_->detect(request);
            callback(makeResponse(response));
            
        } catch (const std::exception& e) {
            sendError(callback, e.what());
        }
    }

private:
    std::shared_ptr<IDetectionService> service_;
    std::shared_ptr<const AppConfig> config_;
};
```

### ModelController

**Before:**
```cpp
class ModelController : public HttpController {
    void GET(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)> callback) {
        // Return hardcoded model list
    }
};
```

**After:**
```cpp
class ModelController : public HttpController {
public:
    explicit ModelController(
        std::shared_ptr<IModelService> service,
        std::shared_ptr<const AppConfig> config)
        : service_(std::move(service)),
          config_(std::move(config)) {}

    void GET(const HttpRequestPtr& req,
             std::function<void(const HttpResponsePtr&)> callback) {
        Json::Value response;
        response["models"] = Json::arrayValue;
        
        for (const auto& model : config_->models().allModels()) {
            Json::Value modelJson;
            modelJson["id"] = model.modelId;
            modelJson["type"] = model.modelType;
            modelJson["strategy"] = model.detectionStrategy;
            modelJson["classes"] = static_cast<int>(model.classLabels.size());
            modelJson["threshold"] = model.confidenceThreshold;
            
            response["models"].append(modelJson);
        }
        
        callback(makeJsonResponse(response));
    }

private:
    std::shared_ptr<IModelService> service_;
    std::shared_ptr<const AppConfig> config_;
};
```

---

## 5. HTTP Server Integration

### Before (Hardcoded)

```cpp
class HttpServer {
    void start() {
        app_.listen("0.0.0.0", 8080);  // Hardcoded
    }
};
```

### After (Injected Config)

```cpp
class HttpServer {
public:
    explicit HttpServer(std::shared_ptr<const AppConfig> config)
        : config_(std::move(config)) {}

    void start() {
        const auto& server = config_->server();
        app_.listen(server.host, server.port);
        std::cout << "Server listening on " << server.host 
                  << ":" << server.port << std::endl;
    }

    int getThreadCount() const {
        return config_->server().threadCount;
    }

private:
    std::shared_ptr<const AppConfig> config_;
};
```

---

## 6. Configuration File Setup

### Step 1: Create config.json from example

```bash
cd /path/to/spectra-vision
cp config.json.example config.json
```

### Step 2: Edit config.json for your environment

```json
{
  "server": {
    "host": "0.0.0.0",
    "port": 8080,
    "threadCount": 32,
    "logPath": "./logs",
    "logLevel": "info"
  },
  "security": {
    "enableAuth": true,
    "rateLimitRps": 100,
    "jwtSecret": "your-production-secret-key"
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
      "modelPath": "/opt/models/emotion_recognition.onnx",
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
    },
    {
      "modelId": "face-detector-v2",
      "modelPath": "/opt/models/face_detector_yolov5.onnx",
      "modelType": "onnx",
      "detectionStrategy": "yolo",
      "classLabels": ["face"],
      "inputShape": {
        "batch": 1,
        "channels": 3,
        "height": 640,
        "width": 640
      },
      "confidenceThreshold": 0.5,
      "nmsThreshold": 0.45
    }
  ]
}
```

---

## 7. Testing Integration

### Unit Test with Injected Config

```cpp
#include <gtest/gtest.h>
#include "config/AppConfig.h"
#include "services/DetectionService.h"

class DetectionServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<const AppConfig> createTestConfig() {
        std::vector<config::ModelConfig> models;
        config::ModelConfig model;
        model.modelId = "test-model";
        model.modelPath = "test.onnx";
        model.modelType = "onnx";
        model.detectionStrategy = "emotion-classification";
        model.classLabels = {"happy", "sad"};
        model.inputShape = {1, 3, 224, 224};
        models.push_back(model);
        
        auto registry = config::ModelRegistry(std::move(models));
        auto cfg = std::make_shared<const AppConfig>(
            config::ServerConfig(),
            config::SecurityConfig(),
            config::PreprocessingConfig(),
            std::move(registry)
        );
        return cfg;
    }
};

TEST_F(DetectionServiceTest, DetectWithInjectedConfig) {
    auto config = createTestConfig();
    auto mockDetector = std::make_shared<MockDetector>();
    auto mockPreprocessor = std::make_shared<MockPreprocessor>();
    
    auto service = std::make_shared<DetectionService>(
        mockDetector, mockPreprocessor, config);
    
    DetectionRequest request;
    request.modelId = "test-model";
    // ... populate request
    
    auto response = service->detect(request);
    
    EXPECT_TRUE(response.success);
}
```

### Integration Test with Real Config

```cpp
TEST(IntegrationTest, LoadAndUseRealConfig) {
    auto config = AppConfig::load("config.json.example");
    
    // Verify config loaded
    EXPECT_EQ(config->server().port, 8080);
    EXPECT_GT(config->models().modelCount(), 0);
    
    // Create services
    auto detector = std::make_shared<OnnxDetector>();
    auto preprocessor = std::make_shared<PreprocessingService>(config);
    auto service = std::make_shared<DetectionService>(
        detector, preprocessor, config);
    
    // Use services
    // ... tests
}
```

---

## 8. Middleware Integration

### Logging Middleware

```cpp
class LoggingMiddleware {
public:
    explicit LoggingMiddleware(std::shared_ptr<const AppConfig> config)
        : config_(std::move(config)) {}

    void onRequest(const HttpRequestPtr& req) {
        const auto& logLevel = config_->server().logLevel;
        if (logLevel == "debug") {
            std::cout << "Request: " << req->getPath() << std::endl;
        }
    }

private:
    std::shared_ptr<const AppConfig> config_;
};
```

### Rate Limiting Middleware

```cpp
class RateLimitMiddleware {
public:
    explicit RateLimitMiddleware(std::shared_ptr<const AppConfig> config)
        : config_(std::move(config)),
          maxRps_(config->security().rateLimitRps) {}

    bool checkRateLimit(const std::string& clientId) {
        // Check if client has exceeded rateLimitRps
        return rateLimiter_.allow(clientId, maxRps_);
    }

private:
    std::shared_ptr<const AppConfig> config_;
    int maxRps_;
    RateLimiter rateLimiter_;
};
```

---

## 9. Migration Checklist

- [ ] Add `#include "config/AppConfig.h"` to main.cpp
- [ ] Load config at startup: `auto config = AppConfig::load("config.json");`
- [ ] Update all service constructors to accept `shared_ptr<const AppConfig>`
- [ ] Inject config into all services
- [ ] Update controllers to use config for validation
- [ ] Replace hardcoded values with config access
- [ ] Create config.json from config.json.example
- [ ] Edit config.json with production paths and parameters
- [ ] Run tests: `ctest -R Config --verbose`
- [ ] Run integration tests with real config
- [ ] Remove old singleton-based configuration
- [ ] Deploy with config.json alongside executable

---

## 10. Troubleshooting

### "Cannot open config file: config.json"

**Solution:** Ensure config.json exists in working directory:
```bash
pwd
ls -la config.json
```

### "Missing 'server' section"

**Solution:** Ensure config.json has all required sections:
```json
{
  "server": { ... },    // Required
  "models": [ ... ]     // Required
}
```

### "Duplicate model ID"

**Solution:** Check for duplicate modelId values in models array

### "Invalid port number"

**Solution:** Port must be between 1 and 65535

### Service doesn't have access to config

**Solution:** Verify:
1. Config is injected in constructor
2. Config is stored as `std::shared_ptr<const AppConfig>`
3. Config is passed with `std::move()` for efficiency

---

## Performance Optimization Tips

1. **Cache model lookups** for frequently accessed models
   ```cpp
   auto model = config_->models().getModel("emotion-v1");
   for (int i = 0; i < 1000; ++i) {
       if (model) {
           // Use model
       }
   }
   ```

2. **Use const references** in method signatures
   ```cpp
   void process(const AppConfig& cfg) {
       // Not passing shared_ptr in every method
   }
   ```

3. **Minimize config access in hot paths**
   - Load thresholds/parameters at service initialization
   - Cache frequently used values

---

## Summary

The configuration system integrates seamlessly with:

✅ **Services** — Injected via constructor
✅ **Controllers** — Access config for validation/routing
✅ **HTTP Server** — Uses config for port/threads/logging
✅ **Middleware** — Uses config for rate limiting/logging
✅ **Tests** — Test config created manually or loaded from file

All with:
- ✅ Immutable configuration (compile-time enforced)
- ✅ Thread-safe concurrent reads (no locks needed)
- ✅ Dependency injection (no global state)
- ✅ Production-ready validation (fail fast at startup)
