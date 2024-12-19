// Copyright (C) 2024. All rights reserved.

#include "gtest/gtest.h"

#include <fstream>
#include <iostream>

#include "runtime/core/configurator/configurator_manager.h"

namespace nxpilot::runtime::core::configurator {

const std::filesystem::path kConfiguratorManagerTestPath = "./configurator_manager_test_cfg.yaml";

class ConfiguratorManagerTest : public ::testing::Test {
 protected:
  static void SetUpTestCase() {
    const auto* cfg_content = R"str(
    nxpilot:
      log:
        backends:
         - type: rotate_file
           options:
             path: /tmp/log
             filename: cpp_logger_rotate_file.log
             max_file_size_mb: 4
             max_file_num: 10
    )str";

    std::ofstream outfile;
    outfile.open(kConfiguratorManagerTestPath, std::ios::out);
    outfile << cfg_content;
    outfile.close();
  }

  static void TearDownTestCase() {
    std::error_code error;
    auto file_status = std::filesystem::status(kConfiguratorManagerTestPath, error);
    if (std::filesystem::exists(file_status)) {
      std::filesystem::remove(kConfiguratorManagerTestPath);
    }
  }

  void SetUp() override { configurator_manager_.Initialize(kConfiguratorManagerTestPath); }

  void TearDown() override { configurator_manager_.Shutdown(); }

  ConfiguratorManager configurator_manager_;
};

TEST_F(ConfiguratorManagerTest, initialize) {
  YAML::Node node = configurator_manager_.GetRootOptionsNode();
  EXPECT_EQ(node.IsNull(), false);
  EXPECT_EQ(node.IsDefined(), true);
}

TEST_F(ConfiguratorManagerTest, start_and_shutdown) {
  configurator_manager_.Start();
  EXPECT_EQ(configurator_manager_.GetState(), ConfiguratorManager::State::kStart);

  configurator_manager_.Shutdown();
  EXPECT_EQ(configurator_manager_.GetState(), ConfiguratorManager::State::kShutdown);
}

TEST_F(ConfiguratorManagerTest, get_value) {
  YAML::Node node = configurator_manager_.GetNodeOptionsByKey("log");
  for (const auto& item : node["backends"]) {
    if (item["type"]) {
      EXPECT_EQ(item["type"].as<std::string>(), "rotate_file");
    }

    if (item["options"]) {
      EXPECT_EQ(item["options"]["path"].as<std::string>(), "/tmp/log");
      EXPECT_EQ(item["options"]["filename"].as<std::string>(), "cpp_logger_rotate_file.log");
      EXPECT_EQ(item["options"]["max_file_size_mb"].as<int>(), 4);
      EXPECT_EQ(item["options"]["max_file_num"].as<int>(), 10);
    }
  }
}

}  // namespace nxpilot::runtime::core::configurator