#pragma once

#include <drogon/HttpSimpleController.h>

namespace spectra::vision::controllers {

// Presentation Layer — liveness and readiness probes for orchestrators (K8s).
// No authentication required; intentionally kept dependency-free.

class HealthController
    : public drogon::HttpSimpleController<HealthController> {
public:
    PATH_LIST_BEGIN
        PATH_ADD("/health/live",  drogon::Get);
        PATH_ADD("/health/ready", drogon::Get);
    PATH_LIST_END

    void asyncHandleHttpRequest(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback) override;
};

} // namespace spectra::vision::controllers
