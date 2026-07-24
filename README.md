# Spectra Vision - Emotion Detection API

A professional C++ emotion detection server showcasing modern design patterns and clean architecture.

## 🎭 Features

- **Strategy Pattern**: Multiple emotion detection algorithms
- **Factory Pattern**: Dynamic model instantiation  
- **Singleton Pattern**: Thread-safe logger
- **Dependency Injection**: Decoupled component design
- **REST API**: HTTP server with JSON responses
- **Cross-Platform**: Windows, Linux, macOS support

## 🏗️ Architecture

### Design Patterns Implemented

1. **Strategy Pattern** (`backend/include/emotion_detection.hpp`)
   - `EmotionDetectionStrategy`: Abstract interface
   - `MockEmotionDetector`: Simple implementation
   - `AdvancedEmotionDetector`: Realistic detector

2. **Factory Pattern** (`backend/include/model_factory.hpp`)
   - Creates appropriate strategy based on model ID
   - Centralized model management

3. **Singleton Pattern** (`backend/include/logger.hpp`)
   - Thread-safe logger using Meyer's Singleton
   - Single instance throughout application lifetime

4. **Dependency Injection** (`backend/include/api_handler.hpp`)
   - Handler receives detector instances
   - Loose coupling between components

### Project Structure

```
spectra-vision/
├── backend/                         # C++ backend
│   ├── include/
│   │   ├── types.hpp
│   │   ├── emotion_detection.hpp
│   │   ├── model_factory.hpp
│   │   ├── logger.hpp
│   │   └── api_handler.hpp
│   ├── src/
│   │   ├── emotion_detection.cpp
│   │   ├── model_factory.cpp
│   │   ├── logger.cpp
│   │   ├── api_handler.cpp
│   │   └── main.cpp
│   ├── external/                     # Header-only libraries
│   │   ├── cpp-httplib/
│   │   └── nlohmann-json/
│   └── CMakeLists.txt
├── frontend/                         # React/Next.js frontend (Vercel)
├── server.py                         # Python Flask fallback
├── client.html                       # HTML test client
├── vercel.json                       # Vercel deployment config
├── build.bat                         # Windows build script
├── run.bat                           # Windows run script
└── README.md
```

## 🚀 Quick Start

### Backend (C++)

**Requirements:**
- CMake 3.20+
- C++17 compiler
- Ninja or Make build tool

**Build:**
```bash
cd backend
mkdir build && cd build
cmake -G "Ninja" -DCMAKE_CXX_STANDARD=17 ..
cmake --build . --parallel 4
```

**Run:**
```bash
./bin/SpectraVision
# Server listens on http://0.0.0.0:8080
```

**Windows:**
```batch
build.bat    # Builds with CMake + Ninja
run.bat      # Runs the compiled binary
dev.bat      # Builds and runs together
```

### Frontend (Python/HTML)

**Quick demo:**
```bash
python server.py
# Access http://localhost:8080/client.html
```

## 📡 API Endpoints

### Health Check
```
GET /health
```
Response:
```json
{
  "status": "ok",
  "service": "Spectra Vision",
  "version": "2.0.0",
  "buildType": "Release"
}
```

### List Models
```
GET /api/v1/models
```

### Emotion Detection
```
POST /api/v1/emotion
Content-Type: application/json

{
  "modelId": "emotion-v1",
  "imageBase64": "base64-encoded-image"
}
```

Response:
```json
{
  "classLabel": "Happy",
  "classId": 0,
  "confidence": 0.94,
  "inferenceTimeMs": 42,
  "timestamp": "2026-07-24T15:22:44"
}
```

## 🏛️ Design Patterns in Depth

### 1. Strategy Pattern
Interchangeable emotion detection algorithms:
```cpp
// Interface
class EmotionDetectionStrategy {
    virtual EmotionResult detect(const std::string& imageData) = 0;
};

// Concrete implementations
class MockEmotionDetector : public EmotionDetectionStrategy { ... };
class AdvancedEmotionDetector : public EmotionDetectionStrategy { ... };

// Usage
auto detector = EmotionModelFactory::create("emotion-v2");
auto result = detector->detect(imageData);
```

### 2. Factory Pattern
Centralizes object creation and model selection:
```cpp
class EmotionModelFactory {
public:
    static std::shared_ptr<EmotionDetectionStrategy> 
    create(const std::string& modelId);
};

// Client code - decoupled from concrete types
auto detector = EmotionModelFactory::create(userSelectedModel);
```

### 3. Singleton Pattern
Thread-safe logger with Meyer's Singleton:
```cpp
class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;  // Guaranteed thread-safe in C++11+
        return instance;
    }
    
    // Copy/move operations deleted to prevent duplication
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};

// Usage
Logger::getInstance().info("Server started");
```

### 4. Dependency Injection
Components receive their dependencies:
```cpp
class APIHandler {
public:
    // Handler doesn't create detectors - they're passed in
    json handleEmotionDetection(const json& request) {
        auto detector = EmotionModelFactory::create(modelId);
        auto result = detector->detect(imageData);
        return formatResponse(result);
    }
};
```

## 🔧 Build Targets

- `SpectraVision`: Main server executable

## 📦 Dependencies

All external dependencies are **header-only**:

- **cpp-httplib**: Lightweight HTTP server
- **nlohmann/json**: Modern JSON library

## 🧪 Testing

Test with curl:
```bash
# Health check
curl http://localhost:8080/health

# List models
curl http://localhost:8080/api/v1/models

# Emotion detection
curl -X POST http://localhost:8080/api/v1/emotion \
  -H "Content-Type: application/json" \
  -d '{"modelId":"emotion-v1","imageBase64":"test"}'
```

## 🌐 Deployment

### Frontend (Vercel)
```bash
npm install
vercel deploy
```

### Backend (Railway/AWS/Docker)

**Docker:**
```dockerfile
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y cmake g++ ninja-build
COPY . /app
WORKDIR /app/backend
RUN mkdir build && cd build && \
    cmake .. && cmake --build .
EXPOSE 8080
CMD ["./build/bin/SpectraVision"]
```

**Railway/AWS EC2:**
1. Compile on platform
2. Upload binary
3. Run: `./SpectraVision`

## 💻 Key Implementation Details

### Error Handling
Exception hierarchy for type-safe error handling:
```cpp
class Exception : public std::runtime_error { };
class ModelException : public Exception { };
class InferenceException : public Exception { };
```

### Resource Management (RAII)
Smart pointers and standard containers manage all resources:
```cpp
std::shared_ptr<EmotionDetectionStrategy> detector = 
    EmotionModelFactory::create(modelId);
// Automatically cleaned up when detector goes out of scope
```

### Thread Safety
Logger uses static instance initialization (thread-safe in C++11):
```cpp
static Logger instance;  // Guaranteed thread-safe creation
```

## 📊 Performance

- Lightweight HTTP server (cpp-httplib)
- Minimal external dependencies
- Header-only libraries (no linking overhead)
- RAII for efficient resource usage
- ~5-100ms inference time (simulated)

## 🎯 Design Principles Applied

✅ SOLID Principles
- Single Responsibility: Each class has one reason to change
- Open/Closed: Extensible through interfaces
- Liskov Substitution: Detectors interchangeable
- Interface Segregation: Clean, focused interfaces
- Dependency Inversion: Depend on abstractions

✅ Modern C++
- C++17 features (shared_ptr, auto, range-based for)
- RAII for resource management
- No raw pointers
- Move semantics where beneficial

✅ Clean Code
- Descriptive names
- Small, focused functions
- DRY principle
- Comprehensive comments

## 📝 Code Statistics

- **Lines of Code**: ~600 (backend)
- **Source Files**: 5 (core implementation)
- **Header Files**: 5 (interfaces & types)
- **Design Patterns**: 4 major patterns
- **External Dependencies**: 2 (header-only)

## 🤝 Contributing

Contributions welcome! Future enhancements:
- [ ] Streaming emotion detection
- [ ] Model caching
- [ ] Metrics/Prometheus export
- [ ] Authentication/Authorization
- [ ] GraphQL API alternative
- [ ] GPU acceleration support

## 📄 License

MIT License

## 🙋 Support

GitHub Issues: [spectra-vision/issues](https://github.com/yourusername/spectra-vision/issues)
