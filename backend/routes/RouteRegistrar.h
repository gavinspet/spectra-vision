#pragma once

#include <drogon/drogon.h>

namespace spectra::vision::routes {

// Presentation Layer — single point of truth for route registration.
// Keeps main.cpp clean; wires controllers to the Drogon app instance.

class RouteRegistrar {
public:
    RouteRegistrar() = delete;

    static void registerAll(drogon::HttpAppFramework& app);

private:
    static void registerApiV1(drogon::HttpAppFramework& app);
    static void registerHealthRoutes(drogon::HttpAppFramework& app);
};

} // namespace spectra::vision::routes
