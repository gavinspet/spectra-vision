#include "Frame.h"

#include <stdexcept>

namespace spectra::vision::models {

Frame::Frame(int width, int height, ColorSpace colorSpace)
    : width_(width), height_(height), colorSpace_(colorSpace) {}

Frame::Frame(int width, int height, ColorSpace colorSpace,
             std::vector<uint8_t> data, FrameMetadata metadata)
    : width_(width), height_(height), colorSpace_(colorSpace),
      data_(std::move(data)), metadata_(std::move(metadata)) {}

int        Frame::width()      const noexcept { return width_; }
int        Frame::height()     const noexcept { return height_; }
ColorSpace Frame::colorSpace() const noexcept { return colorSpace_; }
bool       Frame::empty()      const noexcept { return data_.empty(); }

const FrameMetadata&        Frame::metadata() const noexcept { return metadata_; }
const std::vector<uint8_t>& Frame::data()     const noexcept { return data_; }

void Frame::setMetadata(FrameMetadata meta) { metadata_ = std::move(meta); }
void Frame::setData(std::vector<uint8_t> data) { data_ = std::move(data); }

} // namespace spectra::vision::models
