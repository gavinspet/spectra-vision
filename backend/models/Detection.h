#pragma once

#include <string>

namespace spectra::vision::models {

// Domain Layer — a single inferred detection result.
// This is the canonical domain entity returned by all detector backends.

struct BoundingRect {
    float x{0.0f};
    float y{0.0f};
    float width{0.0f};
    float height{0.0f};
};

struct Detection {
    std::string classLabel;
    int         classId{-1};
    float       confidence{0.0f};
    BoundingRect bbox{};
};

} // namespace spectra::vision::models
