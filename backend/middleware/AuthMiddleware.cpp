#include "AuthMiddleware.h"
#include <drogon/HttpResponse.h>

namespace spectra::vision::middleware {

void AuthMiddleware::doFilter(
    const drogon::HttpRequestPtr&  req,
    drogon::FilterCallback&&       fcb,
    drogon::FilterChainCallback&&  fccb)
{
    // TODO: extract Authorization header
    // TODO: verify Bearer token via isValidToken()
    // TODO: fcb(401 response) on failure; fccb() on success
    fccb();
}

bool AuthMiddleware::isValidToken(const std::string& /*token*/) const {
    // TODO: validate JWT signature / expiry
    return true;
}

} // namespace spectra::vision::middleware
