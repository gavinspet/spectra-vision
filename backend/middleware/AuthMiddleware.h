#pragma once

#include <drogon/HttpFilter.h>

namespace spectra::vision::middleware {

// Presentation Layer — validates Bearer tokens on protected endpoints.
// Plugged into controller route definitions via Drogon's filter mechanism.

class AuthMiddleware : public drogon::HttpFilter<AuthMiddleware> {
public:
    void doFilter(const drogon::HttpRequestPtr&   req,
                  drogon::FilterCallback&&         fcb,
                  drogon::FilterChainCallback&&    fccb) override;

private:
    bool isValidToken(const std::string& token) const;
};

} // namespace spectra::vision::middleware
