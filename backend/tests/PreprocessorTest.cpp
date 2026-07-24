#include <gtest/gtest.h>
#include "preprocessing/ResizePreprocessor.h"
#include "preprocessing/NormalizationPreprocessor.h"
#include "services/PreprocessingService.h"
#include "models/Frame.h"

namespace spectra::vision::tests {

// ── ResizePreprocessor ─────────────────────────────────────────────────────

class ResizePreprocessorTest : public ::testing::Test {
protected:
    preprocessing::ResizePreprocessor sut_{640, 640};
};

TEST_F(ResizePreprocessorTest, Name_IsCorrect) {
    EXPECT_EQ(sut_.name(), "ResizePreprocessor");
}

TEST_F(ResizePreprocessorTest, Preprocess_EmptyFrame_ReturnsEmpty) {
    models::Frame empty;
    // TODO: assert result is empty or resized to 640x640
    SUCCEED();
}

// ── NormalizationPreprocessor ──────────────────────────────────────────────

class NormalizationPreprocessorTest : public ::testing::Test {
protected:
    preprocessing::NormalizationPreprocessor sut_{1.0f / 255.0f};
};

TEST_F(NormalizationPreprocessorTest, Name_IsCorrect) {
    EXPECT_EQ(sut_.name(), "NormalizationPreprocessor");
}

// ── PreprocessingService pipeline ─────────────────────────────────────────

TEST(PreprocessingServiceTest, Pipeline_RunsStepsInOrder) {
    // TODO: build a pipeline with ordered mock steps, verify call order
    SUCCEED();
}

} // namespace spectra::vision::tests
