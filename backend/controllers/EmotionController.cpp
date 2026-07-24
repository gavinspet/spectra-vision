#include "EmotionController.h"
#include "dto/DetectionRequest.h"
#include "dto/DetectionResponse.h"
#include "exceptions/ValidationException.h"
#include "exceptions/SpectraException.h"
#include "logger/Logger.h"
#include <drogon/HttpResponse.h>
#include <json/json.h>

using namespace spectra::vision;

EmotionController::EmotionController(
    std::shared_ptr<interfaces::IDetectionService> service)
    : service_(std::move(service)) {}

void EmotionController::recognize(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    try {
        auto jsonBody = req->getJsonObject();
        if (!jsonBody) {
            throw exceptions::ValidationException(
                "body", "Request body must be valid JSON");
        }

        const auto& obj = *jsonBody;

        if (!obj.isMember("modelId") || !obj["modelId"].isString()) {
            throw exceptions::ValidationException(
                "modelId", "Field required and must be string");
        }
        if (!obj.isMember("imageBase64") || !obj["imageBase64"].isString()) {
            throw exceptions::ValidationException(
                "imageBase64", "Field required and must be base64-encoded string");
        }

        dto::DetectionRequest request;
        request.modelId = obj["modelId"].asString();
        request.imageBase64 = obj["imageBase64"].asString();

        if (!request.modelId.empty() && request.modelId.length() > 256) {
            throw exceptions::ValidationException(
                "modelId", "Must not exceed 256 characters");
        }
        if (request.imageBase64.empty() || request.imageBase64.length() > 10 * 1024 * 1024) {
            throw exceptions::ValidationException(
                "imageBase64", "Image size exceeds maximum 10MB");
        }

        if (obj.isMember("confidenceThreshold")) {
            if (!obj["confidenceThreshold"].isNumeric()) {
                throw exceptions::ValidationException(
                    "confidenceThreshold", "Must be a number between 0 and 1");
            }
            float threshold = obj["confidenceThreshold"].asFloat();
            if (threshold < 0.0f || threshold > 1.0f) {
                throw exceptions::ValidationException(
                    "confidenceThreshold", "Must be between 0 and 1");
            }
            request.confidenceThreshold = threshold;
        }

        if (obj.isMember("maxResults")) {
            if (!obj["maxResults"].isInt()) {
                throw exceptions::ValidationException(
                    "maxResults", "Must be an integer");
            }
            int max = obj["maxResults"].asInt();
            if (max <= 0 || max > 1000) {
                throw exceptions::ValidationException(
                    "maxResults", "Must be between 1 and 1000");
            }
            request.maxResults = max;
        }

        dto::DetectionResponse response = service_->detect(request);

        Json::Value respJson;
        respJson["modelId"] = response.modelId;
        respJson["inferenceTimeMs"] = static_cast<Json::Value::Int64>(response.inferenceTimeMs);

        Json::Value detections(Json::arrayValue);
        for (const auto& obj : response.detections) {
            Json::Value detection;
            detection["classLabel"] = obj.classLabel;
            detection["classId"] = obj.classId;
            detection["confidence"] = obj.confidence;
            detection["bbox"]["x"] = obj.bbox.x;
            detection["bbox"]["y"] = obj.bbox.y;
            detection["bbox"]["width"] = obj.bbox.width;
            detection["bbox"]["height"] = obj.bbox.height;
            detections.append(detection);
        }
        respJson["detections"] = detections;

        auto resp = drogon::HttpResponse::newHttpJsonResponse(respJson);
        resp->setStatusCode(drogon::HttpStatusCode::k200Ok);
        callback(resp);

    } catch (const exceptions::ValidationException& e) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(
            Json::Value{{"error", e.what()}});
        resp->setStatusCode(drogon::HttpStatusCode::k400BadRequest);
        callback(resp);

    } catch (const exceptions::SpectraException& e) {
        Json::Value body;
        body["error"] = e.what();
        auto resp = drogon::HttpResponse::newHttpJsonResponse(body);
        resp->setStatusCode(static_cast<drogon::HttpStatusCode>(e.httpStatusCode()));
        callback(resp);

    } catch (const std::exception& e) {
        Json::Value body;
        body["error"] = "Internal server error";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(body);
        resp->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);
        callback(resp);
    }
}
