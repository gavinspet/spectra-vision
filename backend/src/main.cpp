#include <iostream>
#include <memory>
#include "logger.hpp"
#include "api_handler.hpp"
#include "model_factory.hpp"
#include <nlohmann/json.hpp>
#include <httplib.h>

using json = nlohmann::json;

int main() {
    auto& logger = sv::Logger::getInstance();
    
    logger.info("============================================================");
    logger.info("Spectra Vision - Emotion Detection Server");
    logger.info("Version: 2.0.0");
    logger.info("============================================================");
    
    // Create HTTP server
    httplib::Server svr;
    
    // Create API handler (Dependency Injection)
    sv::APIHandler handler;
    
    // Health check endpoint
    svr.Get("/health", [&handler, &logger](const httplib::Request&, httplib::Response& res) {
        logger.info("GET /health");
        auto response = handler.handleHealthCheck();
        res.set_content(response.dump(), "application/json");
        res.status = 200;
    });
    
    // Get available models
    svr.Get("/api/v1/models", [&handler, &logger](const httplib::Request&, httplib::Response& res) {
        logger.info("GET /api/v1/models");
        auto response = handler.handleListModels();
        res.set_content(response.dump(), "application/json");
        res.status = 200;
    });
    
    // Emotion detection endpoint
    svr.Post("/api/v1/emotion", [&handler, &logger](const httplib::Request& req, httplib::Response& res) {
        logger.info("POST /api/v1/emotion");
        
        try {
            auto request = json::parse(req.body);
            auto response = handler.handleEmotionDetection(request);
            
            res.set_content(response.dump(), "application/json");
            res.status = response.contains("error") ? 400 : 200;
        } catch (const std::exception& e) {
            logger.error("Failed to parse request: " + std::string(e.what()));
            json error = {
                {"error", "Invalid JSON"},
                {"status", "error"}
            };
            res.set_content(error.dump(), "application/json");
            res.status = 400;
        }
    });
    
    // Root endpoint
    svr.Get("/", [&logger](const httplib::Request&, httplib::Response& res) {
        logger.info("GET /");
        json response = {
            {"name", "Spectra Vision Server"},
            {"version", "2.0.0"},
            {"endpoints", {
                {"health", "GET /health"},
                {"models", "GET /api/v1/models"},
                {"emotion_detection", "POST /api/v1/emotion"}
            }}
        };
        res.set_content(response.dump(), "application/json");
        res.status = 200;
    });
    
    // CORS middleware
    svr.set_post_routing_handler([](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
    });
    
    logger.info("Starting server on 0.0.0.0:8080...");
    logger.info("[OK] GET  /health");
    logger.info("[OK] GET  /api/v1/models");
    logger.info("[OK] POST /api/v1/emotion");
    logger.info("============================================================");
    
    if (svr.listen("0.0.0.0", 8080)) {
        logger.critical("Failed to start server");
        return 1;
    }
    
    return 0;
}
