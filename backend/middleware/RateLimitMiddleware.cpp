#include "RateLimitMiddleware.h"
#include <drogon/HttpResponse.h>

namespace spectra::vision::middleware {

RateLimitMiddleware::RateLimitMiddleware(int maxRequestsPerSecond)
    : maxRequestsPerSecond_(maxRequestsPerSecond) {}

void RateLimitMiddleware::doFilter(
    const drogon::HttpRequestPtr&  req,
    drogon::FilterCallback&&       fcb,
    drogon::FilterChainCallback&&  fccb)
{
    // TODO: look up per-IP token bucket (thread-safe map)
    // TODO: fcb(429 response) if bucket exhausted; else consume token + fccb()
    fccb();
}

} // namespace spectra::vision::middleware
