# Configuration System — Quick Reference

## Quick Start

### 1. Load Configuration at Startup

```cpp
#include "config/AppConfig.h"

auto config = AppConfig::load("config.json");
```

### 2. Inject Into Services

```cpp
auto service = std::make_shared<MyService>(config);
```

### 3. Use Configuration

```cpp
// In MyService constructor
MyService::MyService(std::shared_ptr<const AppConfig> cfg)
    : config_(std::move(cfg)) {}

// In MyService methods
void MyService::process() {
    const auto& server = config_->server();
    const auto& models = config_->models();

    if (auto model = models.getModel("emotion-v1")) {
        float threshold = model->get().confidenceThreshold;
        // Use threshold...
    }
}
```

---

## Common Patterns

### Access Server Configuration

```cpp
const auto& srv = config->server();
std::string host = srv.host;
int port = srv.port;
int threads = srv.threadCount;
```

### Access Security Configuration

```cpp
const auto& sec = config->security();
bool authEnabled = sec.enableAuth;
int rateLimitRps = sec.rateLimitRps;
std::string secret = sec.jwtSecret;
```

### Access Preprocessing Configuration

```cpp
const auto& prep = config->preprocessing();
int width = prep.targetWidth;
int height = prep.targetHeight;
float scale = prep.normalizeScale;
float mean_r = prep.meanR;
```

### Look Up Model by ID

```cpp
const auto& models = config->models();

if (auto result = models.getModel("emotion-v1")) {
    const ModelConfig& model = result->get();
    // Use model configuration
} else {
    // Model not found
}
```

### Check if Model Exists

```cpp
if (config->models().hasModel("emotion-v1")) {
    // Model is registered
}
```

### Iterate All Models

```cpp
for (const auto& model : config->models().allModels()) {
    std::cout << "Model: " << model.modelId << std::endl;
}
```

### Get List of Model IDs

```cpp
std::vector<std::string> ids = config->models().listModelIds();
for (const auto& id : ids) {
    std::cout << id << std::endl;
}
```

---

## Error Handling

### Handle Loading Errors

```cpp
try {
    auto config = AppConfig::load("config.json");
    // Use config
} catch (const std::runtime_error& e) {
    // File not found or JSON parse error
    std::cerr << "Failed to load config: " << e.what() << std::endl;
    return 1;
} catch (const std::invalid_argument& e) {
    // Validation error
    std::cerr << "Invalid config: " << e.what() << std::endl;
    return 1;
}
```

### Common Error Messages

```
File errors:
  "Cannot open config file: config.json"
  
JSON parse errors:
  "Invalid JSON in config.json: ..."
  
Validation errors:
  "Missing 'server' section"
  "Missing or invalid field: port"
  "Invalid port number: 0"
  "Thread count must be at least 1, got: 0"
  "Rate limit must be positive, got: -1"
  "Duplicate model ID: emotion-v1"
  "Missing or invalid 'classLabels'"
  "Confidence threshold must be in [0, 1], got: 1.5"
```

---

## Configuration File Template

### Minimal Configuration

```json
{
  "server": {
    "host": "0.0.0.0",
    "port": 8080,
    "threadCount": 16,
    "logPath": "./logs",
    "logLevel": "info"
  },
  "models": [
    {
      "modelId": "emotion-v1",
      "modelPath": "models/emotion.onnx",
      "modelType": "onnx",
      "detectionStrategy": "emotion-classification",
      "classLabels": ["happy", "sad", "neutral"],
      "inputShape": {
        "batch": 1,
        "channels": 3,
        "height": 224,
        "width": 224
      },
      "confidenceThreshold": 0.5,
      "nmsThreshold": 0.45
    }
  ]
}
```

### Full Configuration

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
    "jwtSecret": "your-secret-key"
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
      "modelPath": "models/emotion.onnx",
      "modelType": "onnx",
      "detectionStrategy": "emotion-classification",
      "classLabels": ["angry", "disgust", "fear", "happy", "neutral", "sad"],
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
      "modelPath": "models/face_detector.onnx",
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

## Dependency Injection Examples

### Example 1: Single Service

```cpp
class DetectionService {
public:
    DetectionService(
        std::shared_ptr<IDetector> detector,
        std::shared_ptr<const AppConfig> config)
        : detector_(std::move(detector)),
          config_(std::move(config)) {}

private:
    std::shared_ptr<IDetector> detector_;
    std::shared_ptr<const AppConfig> config_;
};

// Usage
auto config = AppConfig::load("config.json");
auto detector = std::make_shared<OnnxDetector>();
auto service = std::make_shared<DetectionService>(detector, config);
```

### Example 2: Multiple Services Sharing Config

```cpp
auto config = AppConfig::load("config.json");

auto detector = std::make_shared<OnnxDetector>(config);
auto preprocessor = std::make_shared<PreprocessingService>(config);
auto detection = std::make_shared<DetectionService>(detector, preprocessor, config);
auto models = std::make_shared<ModelService>(config);
auto controller = std::make_shared<DetectionController>(detection, models, config);
```

### Example 3: With Factory

```cpp
class ServiceFactory {
public:
    ServiceFactory(std::shared_ptr<const AppConfig> cfg)
        : config_(std::move(cfg)) {}

    std::shared_ptr<IDetector> createDetector() {
        return std::make_shared<OnnxDetector>(config_);
    }

    std::shared_ptr<IPreprocessor> createPreprocessor() {
        return std::make_shared<PreprocessingService>(config_);
    }

private:
    std::shared_ptr<const AppConfig> config_;
};

// Usage
auto config = AppConfig::load("config.json");
ServiceFactory factory(config);
auto detector = factory.createDetector();
auto preprocessor = factory.createPreprocessor();
```

---

## Testing with Configuration

### Mock Configuration for Tests

```cpp
#include <gtest/gtest.h>
#include "config/AppConfig.h"

class MyServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<const AppConfig> createTestConfig() {
        // Create configuration manually for testing
        std::vector<ModelConfig> models;
        ModelConfig model;
        model.modelId = "test-model";
        model.modelPath = "test_model.onnx";
        model.modelType = "onnx";
        model.detectionStrategy = "emotion-classification";
        model.classLabels = {"happy", "sad"};
        models.push_back(model);

        auto registry = ModelRegistry(std::move(models));
        auto config = std::make_shared<const AppConfig>(
            ServerConfig(),
            SecurityConfig(),
            PreprocessingConfig(),
            std::move(registry)
        );
        return config;
    }
};

TEST_F(MyServiceTest, ProcessWithTestConfig) {
    auto config = createTestConfig();
    auto service = std::make_shared<MyService>(config);
    // Test service behavior with known config
}
```

### Load Config from File in Tests

```cpp
TEST(MyServiceTest, ProcessWithRealConfig) {
    auto config = AppConfig::load("config.json.example");
    auto service = std::make_shared<MyService>(config);
    
    // Test with real configuration
    service->process();
}
```

---

## Model Configuration Details

### Input Shape

```json
"inputShape": {
  "batch": 1,          // Batch size (typically 1 for inference)
  "channels": 3,       // Color channels (3 for RGB)
  "height": 224,       // Input height in pixels
  "width": 224         // Input width in pixels
}
```

Access in code:
```cpp
const auto& model = config->models().getModel("emotion-v1")->get();
int batch = model.inputShape.batch;
int height = model.inputShape.height;
```

### Detection Strategies

```json
"detectionStrategy": "emotion-classification"  // Argmax over softmax
"detectionStrategy": "yolo"                     // YOLO decoding
"detectionStrategy": "ssd"                      // SSD decoding
```

### Class Labels

```json
"classLabels": ["angry", "disgust", "fear", "happy", "neutral", "sad", "surprise"]
```

Access:
```cpp
const auto& model = config->models().getModel("emotion-v1")->get();
int numClasses = model.classLabels.size();  // 7
std::string label = model.classLabels[0];    // "angry"
```

### Thresholds

```json
"confidenceThreshold": 0.6,   // Min confidence for detection (0.0-1.0)
"nmsThreshold": 0.45          // NMS suppression threshold (0.0-1.0)
```

Access:
```cpp
float confThreshold = model.confidenceThreshold;
float nmsThreshold = model.nmsThreshold;
```

---

## Performance Tips

1. **Cache model lookups** if called frequently
   ```cpp
   // Instead of this:
   for (int i = 0; i < 1000; ++i) {
       auto model = config->models().getModel("emotion-v1");
   }

   // Do this:
   auto model = config->models().getModel("emotion-v1");
   if (model) {
       for (int i = 0; i < 1000; ++i) {
           // Use model reference
       }
   }
   ```

2. **Pass config by const reference**, not by value
   ```cpp
   // Use const reference in methods
   void process(const AppConfig& cfg) {
       // Not passing shared_ptr in every call
   }
   ```

3. **Use allModels() for full enumeration**
   ```cpp
   // Single allocation + one loop
   for (const auto& model : config->models().allModels()) {
       // Process model
   }
   ```

---

## Troubleshooting

### Config file not found

**Error:** `"Cannot open config file: config.json"`

**Solution:** Ensure config.json exists in the working directory:
```bash
ls -la config.json
```

### JSON parse error

**Error:** `"Invalid JSON in config.json: ..."`

**Solution:** Validate JSON syntax using an online validator or:
```bash
python3 -m json.tool config.json
```

### Missing required field

**Error:** `"Missing 'server' section"`

**Solution:** Ensure your JSON includes all required sections:
- server (required)
- models (required)
- security (optional, has defaults)
- preprocessing (optional, has defaults)

### Model not found at runtime

**Symptom:** `getModel("model-id")` returns std::nullopt

**Solution:** Check model ID in config.json matches exactly:
```cpp
for (const auto& id : config->models().listModelIds()) {
    std::cout << "Available: " << id << std::endl;
}
```

### Invalid threshold values

**Error:** `"Confidence threshold must be in [0, 1], got: 1.5"`

**Solution:** Ensure thresholds are between 0.0 and 1.0 in config.json:
```json
"confidenceThreshold": 0.5,
"nmsThreshold": 0.45
```

---

## API Reference

### AppConfig

```cpp
// Load configuration (only constructor)
static std::shared_ptr<const AppConfig> AppConfig::load(
    const std::string& configFilePath);  // throws std::runtime_error, std::invalid_argument

// Accessors (all const noexcept)
const ServerConfig& server() const noexcept;
const SecurityConfig& security() const noexcept;
const PreprocessingConfig& preprocessing() const noexcept;
const ModelRegistry& models() const noexcept;
```

### ModelRegistry

```cpp
// Constructor
explicit ModelRegistry(std::vector<ModelConfig> models);  // throws std::invalid_argument

// Accessors (all const)
std::optional<std::reference_wrapper<const ModelConfig>>
getModel(const std::string& modelId) const;

bool hasModel(const std::string& modelId) const;
std::vector<std::string> listModelIds() const;
const std::vector<ModelConfig>& allModels() const;
std::size_t modelCount() const;
```

### Value Types

```cpp
struct ServerConfig { /* host, port, threadCount, logPath, logLevel */ };
struct SecurityConfig { /* enableAuth, rateLimitRps, jwtSecret */ };
struct PreprocessingConfig { /* targetWidth, targetHeight, means, scale */ };
struct InputShape { /* batch, channels, height, width */ };
struct ModelConfig { /* modelId, modelPath, modelType, detectionStrategy, etc */ };
```
