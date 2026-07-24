#include "LoggingMiddleware.h"
#include "utils/Logger.h"

namespace spectra::vision::middleware {

void LoggingMiddleware::doFilter(
    const drogon::HttpRequestPtr&  req,
    drogon::FilterCallback&&       fcb,
    drogon::FilterChainCallback&&  fccb)
{
    // TODO: record start time
    // TODO: log method + path
    // TODO: fccb() to continue chain, then log response status + latency
    fccb();
}

} // namespace spectra::vision::middleware
