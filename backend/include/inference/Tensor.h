#pragma once

#include <vector>
#include <string>

namespace spectra::vision::inference {

// Infrastructure Layer — simple, backend-independent tensor representation.
// Decouples inference engines from tensor memory management.
// Supports arbitrary dimensionality and element type (currently float32).
// Design: Copyable value-semantic container, no inference engine types leak out.

class Tensor {
public:
    Tensor() = default;

    Tensor(const std::vector<int64_t>& shape, std::vector<float> data)
        : shape_(shape), data_(std::move(data)) {}

    explicit Tensor(const std::vector<int64_t>& shape)
        : shape_(shape), data_(computeSize(shape), 0.0f) {}

    const std::vector<int64_t>& shape() const noexcept { return shape_; }
    const std::vector<float>& data() const noexcept { return data_; }
    std::vector<float>& data() noexcept { return data_; }

    float* data_ptr() noexcept { return data_.data(); }
    const float* data_ptr() const noexcept { return data_.data(); }

    size_t size() const noexcept { return data_.size(); }
    bool empty() const noexcept { return data_.empty(); }

    int ndim() const noexcept { return static_cast<int>(shape_.size()); }

    int64_t dim(int index) const noexcept {
        return (index >= 0 && index < static_cast<int>(shape_.size()))
                   ? shape_[index]
                   : -1;
    }

private:
    static size_t computeSize(const std::vector<int64_t>& shape) {
        size_t size = 1;
        for (auto dim : shape) {
            if (dim <= 0) return 0;
            size *= static_cast<size_t>(dim);
        }
        return size;
    }

    std::vector<int64_t> shape_;
    std::vector<float>   data_;
};

} // namespace spectra::vision::inference
