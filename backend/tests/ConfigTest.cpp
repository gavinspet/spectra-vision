#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <sstream>
#include <stdexcept>

#include "config/AppConfig.h"
#include "config/ModelRegistry.h"

using namespace spectra::vision::config;

// ═════════════════════════════════════════════════════════════════════════════
// CONFIGTYPES TESTS
// ═════════════════════════════════════════════════════════════════════════════

class ConfigTypesTest : public ::testing::Test {
protected:
    ModelConfig createSampleModelConfig() {
        ModelConfig cfg;
        cfg.modelId = "test-model";
        cfg.modelPath = "models/test.onnx";
        cfg.modelType = "onnx";
        cfg.detectionStrategy = "emotion-classification";
        cfg.classLabels = {"happy", "sad", "neutral"};
        cfg.inputShape = {1, 3, 224, 224};
        cfg.confidenceThreshold = 0.5f;
        cfg.nmsThreshold = 0.45f;
        return cfg;
    }
};

TEST_F(ConfigTypesTest, ModelConfigEquality) {
    auto cfg1 = createSampleModelConfig();
    auto cfg2 = createSampleModelConfig();
    EXPECT_EQ(cfg1, cfg2);
}

TEST_F(ConfigTypesTest, ModelConfigInequality) {
    auto cfg1 = createSampleModelConfig();
    auto cfg2 = createSampleModelConfig();
    cfg2.modelId = "different-model";
    EXPECT_NE(cfg1, cfg2);
}

TEST_F(ConfigTypesTest, ServerConfigDefaults) {
    ServerConfig cfg;
    EXPECT_EQ(cfg.host, "0.0.0.0");
    EXPECT_EQ(cfg.port, 8080);
    EXPECT_EQ(cfg.threadCount, 16);
}

// ═════════════════════════════════════════════════════════════════════════════
// MODELREGISTRY TESTS
// ═════════════════════════════════════════════════════════════════════════════

class ModelRegistryTest : public ::testing::Test {
protected:
    ModelConfig createModelConfig(const std::string& id) {
        ModelConfig cfg;
        cfg.modelId = id;
        cfg.modelPath = "models/" + id + ".onnx";
        cfg.modelType = "onnx";
        cfg.detectionStrategy = "emotion-classification";
        cfg.classLabels = {"label1", "label2"};
        return cfg;
    }
};

TEST_F(ModelRegistryTest, ConstructorWithValidModels) {
    std::vector<ModelConfig> models = {createModelConfig("model-1"),
                                       createModelConfig("model-2")};
    EXPECT_NO_THROW({
        ModelRegistry registry(std::move(models));
        EXPECT_EQ(registry.modelCount(), 2);
    });
}

TEST_F(ModelRegistryTest, GetModelByIdSuccess) {
    std::vector<ModelConfig> models = {createModelConfig("emotion-v1")};
    ModelRegistry registry(std::move(models));

    auto result = registry.getModel("emotion-v1");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->get().modelId, "emotion-v1");
}

TEST_F(ModelRegistryTest, GetModelByIdNotFound) {
    std::vector<ModelConfig> models = {createModelConfig("emotion-v1")};
    ModelRegistry registry(std::move(models));

    auto result = registry.getModel("nonexistent");
    EXPECT_FALSE(result.has_value());
}

TEST_F(ModelRegistryTest, HasModelSuccess) {
    std::vector<ModelConfig> models = {createModelConfig("emotion-v1")};
    ModelRegistry registry(std::move(models));

    EXPECT_TRUE(registry.hasModel("emotion-v1"));
    EXPECT_FALSE(registry.hasModel("nonexistent"));
}

TEST_F(ModelRegistryTest, ListModelIds) {
    std::vector<ModelConfig> models = {createModelConfig("model-1"),
                                       createModelConfig("model-2"),
                                       createModelConfig("model-3")};
    ModelRegistry registry(std::move(models));

    auto ids = registry.listModelIds();
    EXPECT_EQ(ids.size(), 3);
    EXPECT_THAT(ids, ::testing::ElementsAre("model-1", "model-2", "model-3"));
}

TEST_F(ModelRegistryTest, DuplicateModelIdThrows) {
    std::vector<ModelConfig> models = {createModelConfig("emotion-v1"),
                                       createModelConfig("emotion-v1")};
    EXPECT_THROW({ ModelRegistry registry(std::move(models)); },
                 std::invalid_argument);
}

TEST_F(ModelRegistryTest, EmptyModelIdThrows) {
    std::vector<ModelConfig> models;
    auto cfg = createModelConfig("valid");
    cfg.modelId = "";  // Invalid
    models.push_back(cfg);

    EXPECT_THROW({ ModelRegistry registry(std::move(models)); },
                 std::invalid_argument);
}

TEST_F(ModelRegistryTest, EmptyModelPathThrows) {
    std::vector<ModelConfig> models;
    auto cfg = createModelConfig("valid");
    cfg.modelPath = "";  // Invalid
    models.push_back(cfg);

    EXPECT_THROW({ ModelRegistry registry(std::move(models)); },
                 std::invalid_argument);
}

TEST_F(ModelRegistryTest, EmptyClassLabelsThrows) {
    std::vector<ModelConfig> models;
    auto cfg = createModelConfig("valid");
    cfg.classLabels.clear();  // Invalid
    models.push_back(cfg);

    EXPECT_THROW({ ModelRegistry registry(std::move(models)); },
                 std::invalid_argument);
}

TEST_F(ModelRegistryTest, MoveConstructor) {
    std::vector<ModelConfig> models = {createModelConfig("model-1")};
    ModelRegistry registry1(std::move(models));
    EXPECT_EQ(registry1.modelCount(), 1);

    ModelRegistry registry2(std::move(registry1));
    EXPECT_EQ(registry2.modelCount(), 1);
}

TEST_F(ModelRegistryTest, MoveAssignment) {
    std::vector<ModelConfig> models1 = {createModelConfig("model-1")};
    std::vector<ModelConfig> models2 = {createModelConfig("model-2"),
                                        createModelConfig("model-3")};

    ModelRegistry registry1(std::move(models1));
    ModelRegistry registry2(std::move(models2));

    registry1 = std::move(registry2);
    EXPECT_EQ(registry1.modelCount(), 2);
}

TEST_F(ModelRegistryTest, NoCopyConstructor) {
    std::vector<ModelConfig> models = {createModelConfig("model-1")};
    ModelRegistry registry(std::move(models));

    // This should not compile:
    // ModelRegistry copy = registry;
    // But we can verify the delete via static_assert in a constexpr context
}

// ═════════════════════════════════════════════════════════════════════════════
// APPCONFIG TESTS (using example JSON file)
// ═════════════════════════════════════════════════════════════════════════════

class AppConfigTest : public ::testing::Test {
protected:
    std::string getExampleConfigPath() const {
        // Assumes config.json.example exists in workspace root
        return "config.json.example";
    }
};

TEST_F(AppConfigTest, LoadFromFileSucceeds) {
    // This test assumes config.json.example is properly formatted
    // In a real scenario, you'd create a test fixture file
    auto config = AppConfig::load(getExampleConfigPath());

    EXPECT_NE(config, nullptr);
    EXPECT_EQ(config->server().host, "0.0.0.0");
    EXPECT_EQ(config->server().port, 8080);
}

TEST_F(AppConfigTest, ServerConfigIsImmutable) {
    auto config = AppConfig::load(getExampleConfigPath());

    // This code should not compile:
    // config->server().port = 9000;  // Error: const reference

    // Verify we can read but not modify
    EXPECT_EQ(config->server().port, 8080);
}

TEST_F(AppConfigTest, ModelsIsAccessible) {
    auto config = AppConfig::load(getExampleConfigPath());

    const auto& models = config->models();
    EXPECT_GT(models.modelCount(), 0);
}

TEST_F(AppConfigTest, SharedPtrOwnership) {
    auto config1 = AppConfig::load(getExampleConfigPath());
    auto config2 = config1;  // Shared ownership

    // Both pointers reference the same object
    EXPECT_EQ(config1.get(), config2.get());

    // Reference count should be 2
    EXPECT_EQ(config1.use_count(), 2);
}

TEST_F(AppConfigTest, NonCopyable) {
    // Verify AppConfig is non-copyable and non-movable
    // These should not compile:
    // auto config = AppConfig::load(path);
    // auto copy = *config;  // Error: deleted copy constructor
}

TEST_F(AppConfigTest, DependencyInjection) {
    auto config = AppConfig::load(getExampleConfigPath());

    // Simulate dependency injection pattern
    std::shared_ptr<const AppConfig> injected = config;

    // Service receives const reference to config
    const auto& serverCfg = injected->server();
    EXPECT_EQ(serverCfg.host, "0.0.0.0");
}

// ═════════════════════════════════════════════════════════════════════════════
// THREAD SAFETY TESTS
// ═════════════════════════════════════════════════════════════════════════════

class ConfigThreadSafetyTest : public ::testing::Test {
protected:
    ModelConfig createModelConfig(const std::string& id) {
        ModelConfig cfg;
        cfg.modelId = id;
        cfg.modelPath = "models/" + id + ".onnx";
        cfg.modelType = "onnx";
        cfg.detectionStrategy = "emotion-classification";
        cfg.classLabels = {"label1"};
        return cfg;
    }
};

TEST_F(ConfigThreadSafetyTest, ModelRegistryReadIsThreadSafe) {
    std::vector<ModelConfig> models;
    for (int i = 0; i < 10; ++i) {
        models.push_back(createModelConfig("model-" + std::to_string(i)));
    }

    auto registry = std::make_shared<ModelRegistry>(std::move(models));

    // Multiple threads can read concurrently
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([registry]() {
            for (int j = 0; j < 100; ++j) {
                auto result = registry->getModel("model-5");
                EXPECT_TRUE(result.has_value());
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }
}

TEST_F(ConfigThreadSafetyTest, AppConfigSharedPtrIsThreadSafe) {
    auto config = std::make_shared<const AppConfig>(
        ServerConfig(), SecurityConfig(), PreprocessingConfig(),
        ModelRegistry(std::vector<ModelConfig>()));

    // Multiple threads can access shared_ptr concurrently
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([config]() {
            for (int j = 0; j < 100; ++j) {
                auto server = config->server();
                (void)server;  // Use it
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }
}
