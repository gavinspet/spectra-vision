#pragma once

#include <drogon/HttpFilter.h>

namespace spectra::vision::middleware {

// Presentation Layer — structured request/response logging (method, path,
// status, latency). All log output is routed through utils::Logger.

class LoggingMiddleware : public drogon::HttpFilter<LoggingMiddleware> {
public:
    void doFilter(const drogon::HttpRequestPtr&   req,
                  drogon::FilterCallback&&         fcb,
                  drogon::FilterChainCallback&&    fccb) override;
};

} // namespace spectra::vision::middleware
