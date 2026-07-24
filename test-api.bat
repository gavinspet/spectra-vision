@echo off
setlocal enabledelayedexpansion

set BASE_URL=http://localhost:8080

echo Testing Spectra Vision Emotion Detection API
echo =============================================
echo.

echo 1. Testing POST /api/v1/emotion with valid request:
echo.

curl -X POST "%BASE_URL%/api/v1/emotion" ^
  -H "Content-Type: application/json" ^
  -d "{\"modelId\": \"emotion-v1\", \"imageBase64\": \"iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mNk+M9QDwADhgGAWjR9awAAAABJRU5ErkJggg==\", \"confidenceThreshold\": 0.5, \"maxResults\": 10}" ^
  -w "\nHTTP Status: %%{http_code}\n\n"

echo.

echo 2. Testing with missing imageBase64 (should return 400):
echo.

curl -X POST "%BASE_URL%/api/v1/emotion" ^
  -H "Content-Type: application/json" ^
  -d "{\"modelId\": \"emotion-v1\"}" ^
  -w "\nHTTP Status: %%{http_code}\n\n"

echo.

echo 3. Testing with missing modelId (should return 400):
echo.

curl -X POST "%BASE_URL%/api/v1/emotion" ^
  -H "Content-Type: application/json" ^
  -d "{\"imageBase64\": \"iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mNk+M9QDwADhgGAWjR9awAAAABJRU5ErkJggg==\"}" ^
  -w "\nHTTP Status: %%{http_code}\n\n"

echo.

echo 4. Testing with invalid JSON (should return 400):
echo.

curl -X POST "%BASE_URL%/api/v1/emotion" ^
  -H "Content-Type: application/json" ^
  -d "invalid json" ^
  -w "\nHTTP Status: %%{http_code}\n\n"
