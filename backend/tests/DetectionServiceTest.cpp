#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "interfaces/IDetectionService.h"
#include "interfaces/IDetector.h"
#include "interfaces/IPreprocessingService.h"
#include "services/DetectionService.h"
#include "dto/DetectionRequest.h"
#include "dto/DetectionResponse.h"

namespace spectra::vision::tests {

// ── Mocks ──────────────────────────────────────────────────────────────────

class MockDetector : public interfaces::IDetector {
public:
    MOCK_METHOD(void, loadModel, (const std::string&), (override));
    MOCK_METHOD(std::vector<dto::BoundingBox>, detect,
                (const models::Frame&), (override));
    MOCK_METHOD(std::string, getBackendName, (), (const, override));
    MOCK_METHOD(bool, isReady, (), (const, override));
};

class MockPreprocessingService : public interfaces::IPreprocessingService {
public:
    MOCK_METHOD(models::Frame, runPipeline, (const models::Frame&), (const, override));
};

// ── Fixture ────────────────────────────────────────────────────────────────

class DetectionServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockDetector_      = std::make_shared<MockDetector>();
        mockPreprocessor_  = std::make_shared<MockPreprocessingService>();
        sut_ = std::make_unique<services::DetectionService>(
            mockDetector_, mockPreprocessor_);
    }

    std::shared_ptr<MockDetector>            mockDetector_;
    std::shared_ptr<MockPreprocessingService> mockPreprocessor_;
    std::unique_ptr<services::DetectionService> sut_;
};

// ── Tests ──────────────────────────────────────────────────────────────────

TEST_F(DetectionServiceTest, Detect_ReturnsResponse_WhenDetectorReady) {
    // TODO: EXPECT_CALL mockPreprocessor_, mockDetector_
    // TODO: call sut_->detect(request), assert response fields
    SUCCEED();
}

TEST_F(DetectionServiceTest, Detect_PropagatesDetectorException) {
    // TODO: configure mock to throw DetectionException
    // TODO: assert sut_->detect() rethrows
    SUCCEED();
}

} // namespace spectra::vision::tests
