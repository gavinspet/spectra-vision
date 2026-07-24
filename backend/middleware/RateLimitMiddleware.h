#pragma once

#include <drogon/HttpFilter.h>
#include <atomic>
#include <chrono>

namespace spectra::vision::middleware {

// Presentation Layer — sliding-window rate limiter per client IP.
// Returns HTTP 429 when the request rate exceeds the configured ceiling.

class RateLimitMiddleware : public drogon::HttpFilter<RateLimitMiddleware> {
public:
    explicit RateLimitMiddleware(int maxRequestsPerSecond = 100);

    void doFilter(const drogon::HttpRequestPtr&   req,
                  drogon::FilterCallback&&         fcb,
                  drogon::FilterChainCallback&&    fccb) override;

private:
    int maxRequestsPerSecond_;
};

} // namespace spectra::vision::middleware
