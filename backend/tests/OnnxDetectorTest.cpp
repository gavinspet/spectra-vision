#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "detectors/OnnxDetector.h"
#include "strategy/IDetectionStrategy.h"
#include "dto/BoundingBox.h"

namespace spectra::vision::tests {

// ── Mock ───────────────────────────────────────────────────────────────────

class MockDetectionStrategy : public strategy::IDetectionStrategy {
public:
    MOCK_METHOD(std::vector<dto::BoundingBox>, postprocess,
               (const void*, std::size_t, int, int, float, float),
               (const, override));
    MOCK_METHOD(std::string, strategyName, (), (const, override));
};

// ── Fixture ────────────────────────────────────────────────────────────────

class OnnxDetectorTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockStrategy_ = std::make_shared<MockDetectionStrategy>();
        sut_ = std::make_unique<detectors::OnnxDetector>(mockStrategy_);
    }

    std::shared_ptr<MockDetectionStrategy>    mockStrategy_;
    std::unique_ptr<detectors::OnnxDetector>  sut_;
};

// ── Tests ──────────────────────────────────────────────────────────────────

TEST_F(OnnxDetectorTest, IsNotReadyBeforeLoad) {
    EXPECT_FALSE(sut_->isReady());
}

TEST_F(OnnxDetectorTest, GetBackendName_ReturnsOnnxruntime) {
    EXPECT_EQ(sut_->getBackendName(), "onnxruntime");
}

TEST_F(OnnxDetectorTest, LoadModel_ThrowsOnInvalidPath) {
    // TODO: expect ModelLoadException for a non-existent path
    SUCCEED();
}

TEST_F(OnnxDetectorTest, Detect_DelegatesToStrategy) {
    // TODO: load a valid fixture model, EXPECT_CALL strategy postprocess
    SUCCEED();
}

} // namespace spectra::vision::tests
