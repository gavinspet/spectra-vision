# Local Testing Guide - Spectra Vision

## Quick Summary

You have **three options** to test the project:

### ✅ Option 1: Python Server (Works NOW)
- **No compiler needed**
- Tests all endpoints
- Validates architecture

```bash
C:\Users\kartick\AppData\Local\Programs\Python\Python312\python.exe server.py
# Access: http://localhost:8080/health
```

### 🔨 Option 2: C++ Backend (Requires Compiler)
- Install compiler (MinGW or MSVC)
- CMake build process
- Uses cpp-httplib and nlohmann-json

### 🐳 Option 3: Docker (Recommended for Deployment)
- No local compiler needed
- Builds in isolated container
- Same as deployment environment

---

## Testing Locally RIGHT NOW

### **Step 1: Start Python Server**
```bash
cd c:\Users\kartick\projects\spectra-vision
C:\Users\kartick\AppData\Local\Programs\Python\Python312\python.exe server.py
```

Output:
```
[OK] Server listening on 0.0.0.0:8080
[OK] GET /health - Health check
[OK] POST /api/v1/emotion - Detect emotions
```

### **Step 2: Test Health Endpoint**
```powershell
curl http://localhost:8080/health | ConvertFrom-Json | Format-List
```

Response:
```
status  : ok
service : Spectra Vision
version : 1.0.0
```

### **Step 3: Test Emotion Detection**
```powershell
$body = @{
    modelId = "emotion-v1"
    imageBase64 = "test-image-data"
} | ConvertTo-Json

curl -X POST http://localhost:8080/api/v1/emotion `
  -H "Content-Type: application/json" `
  -d $body | ConvertFrom-Json | Format-List
```

Response:
```
classLabel      : Happy
classId         : 0
confidence      : 0.94
inferenceTimeMs : 42
timestamp       : 2026-07-24T15:22:44
```

### **Step 4: Open HTML Client**
```
file:///c:/Users/kartick/projects/spectra-vision/client.html
```

---

## Building C++ Backend Locally

If you want to compile the C++ code locally:

### Install a C++ Compiler

**Option A: MinGW (Free)**
```powershell
# Install via MSYS2 or direct download
# https://www.mingw-w64.org/
```

**Option B: Visual Studio (Free Community Edition)**
```powershell
# Download from https://visualstudio.microsoft.com/vs/community/
# Select "Desktop development with C++"
```

**Option C: LLVM/Clang**
```powershell
winget install LLVM
```

### Build Steps

```bash
# Clean previous build
Remove-Item -Path backend/build -Recurse -Force

# Create build directory
mkdir backend/build
cd backend/build

# Configure with CMake
cmake -G "Visual Studio 17 2022" -A x64 ..

# Build
cmake --build . --parallel 4

# Run
.\bin\SpectraVision.exe
```

---

## Building with Docker

The **easiest way** to compile and test:

### Prerequisites
```powershell
# Install Docker Desktop for Windows
# https://www.docker.com/products/docker-desktop
```

### Build Docker Image
```bash
cd c:\Users\kartick\projects\spectra-vision
docker build -t spectra-vision:latest .
```

### Run Container
```bash
docker run -d -p 8080:8080 spectra-vision:latest
# Access: http://localhost:8080/health
```

### View Logs
```bash
docker logs $(docker ps -q)
```

---

## What You Should Test

### API Endpoints
- ✅ `GET /health` - Returns server status
- ✅ `POST /api/v1/emotion` - Accepts modelId + imageBase64
- ✅ `GET /api/v1/models` - Lists available models

### Response Format
```json
{
  "classLabel": "Happy",
  "classId": 0,
  "confidence": 0.94,
  "inferenceTimeMs": 42,
  "timestamp": "2026-07-24T15:22:44"
}
```

### Design Patterns (Code Review)
- ✅ `backend/include/emotion_detection.hpp` - Strategy Pattern
- ✅ `backend/include/model_factory.hpp` - Factory Pattern
- ✅ `backend/include/logger.hpp` - Singleton Pattern
- ✅ `backend/src/api_handler.cpp` - Dependency Injection

---

## Verification Checklist

- [ ] Python server starts on port 8080
- [ ] Health endpoint responds with `{ status: "ok" }`
- [ ] Emotion endpoint returns emotion data
- [ ] HTML client loads at `client.html`
- [ ] Design patterns visible in C++ code
- [ ] CMakeLists.txt properly configured
- [ ] All external headers downloaded
- [ ] Documentation complete

---

## Next Steps

### 1. **Verify Everything Works**
```bash
# Terminal 1: Start server
python server.py

# Terminal 2: Test endpoints
curl http://localhost:8080/health
curl -X POST http://localhost:8080/api/v1/emotion ...
```

### 2. **Review Code Structure**
```
backend/
├── include/          # 5 headers showcasing patterns
├── src/              # 5 implementation files
├── external/         # Header-only libraries
└── CMakeLists.txt    # Professional build config
```

### 3. **Push to GitHub**
```bash
git init
git add .
git commit -m "Initial: C++ emotion detection with design patterns"
git remote add origin https://github.com/yourusername/spectra-vision
git push -u origin main
```

### 4. **Deploy Backend** (Choose one)
- **Railway.app** → Automatic Docker deployment
- **AWS EC2** → Manual setup
- **Docker Hub** → Container registry

### 5. **Deploy Frontend** (Choose one)
- **Vercel** → Next.js/React (Recommended)
- **Netlify** → Alternative
- **GitHub Pages** → Static only

---

## Troubleshooting

### Python Server Won't Start
```bash
# Check Python installation
python --version

# Check Flask is installed
python -m pip list | grep flask

# If missing, install
python -m pip install flask
```

### API Not Responding
```bash
# Check if server is running
netstat -ano | findstr :8080

# Test connectivity
curl -v http://localhost:8080/health
```

### C++ Compilation Issues
- **No compiler found**: Install Visual Studio Community or MinGW
- **CMake error**: Ensure CMake 3.20+ installed
- **Missing headers**: Verify external/ directory has httplib.h and json.hpp

### Docker Build Failed
```bash
# Check Docker is running
docker --version
docker ps

# Build with verbose output
docker build --progress=plain -t spectra-vision:latest .
```

---

## Performance Testing

### Load Testing
```bash
# Install Apache Bench
winget install Apache.HTTP.Server

# Test with 100 requests, 10 concurrent
ab -n 100 -c 10 http://localhost:8080/health
```

### Response Time
```powershell
# Measure single request
Measure-Object -InputObject $(curl http://localhost:8080/health)
```

---

## Summary

**Right Now:**
1. ✅ Python server is ready to test
2. ✅ All C++ code is in place
3. ✅ Documentation is complete
4. ✅ Deployment configs are set

**To Push to GitHub:**
```bash
git push origin main
```

**Then Deploy:**
- Railway.app (backend) + Vercel (frontend)
- Get live demo URLs
- Share on portfolio

You're ready to go! 🚀
