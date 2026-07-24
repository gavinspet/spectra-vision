#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace spectra::vision::models {

// Domain Layer — the central data-carrier for raw and preprocessed image data.
// Passed through the preprocessing pipeline and consumed by detectors.

enum class ColorSpace { BGR, RGB, GRAY };

struct FrameMetadata {
    std::string  sourceId;
    std::int64_t timestampMs{0};
    int          originalWidth{0};
    int          originalHeight{0};
};

class Frame {
public:
    Frame() = default;
    Frame(int width, int height, ColorSpace colorSpace);
    Frame(int width, int height, ColorSpace colorSpace,
          std::vector<uint8_t> data, FrameMetadata metadata);

    int         width()      const noexcept;
    int         height()     const noexcept;
    ColorSpace  colorSpace() const noexcept;
    bool        empty()      const noexcept;

    const FrameMetadata&        metadata() const noexcept;
    const std::vector<uint8_t>& data()     const noexcept;

    void setMetadata(FrameMetadata meta);
    void setData(std::vector<uint8_t> data);

private:
    int                  width_{0};
    int                  height_{0};
    ColorSpace           colorSpace_{ColorSpace::BGR};
    std::vector<uint8_t> data_;
    FrameMetadata        metadata_{};
};

} // namespace spectra::vision::models
