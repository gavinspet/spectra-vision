#pragma once

namespace spectra::vision::dto {

// Application Layer — represents a detected region in screen space.
// Used in DetectionResponse and decoupled from the domain BoundingRect.

struct BoundingBox {
    float x{0.0f};
    float y{0.0f};
    float width{0.0f};
    float height{0.0f};
};

} // namespace spectra::vision::dto
