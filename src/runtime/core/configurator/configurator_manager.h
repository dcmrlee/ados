// Copyright (C) 2024. All rights reserved.

#pragma once

#include <atomic>
#include <filesystem>

#include "utils/common/log_util.h"
#include "yaml-cpp/yaml.h"

namespace nxpilot::runtime::core::configurator {

class ConfiguratorManager {
 public:
  ConfiguratorManager() : logger_ptr_(std::make_shared<nxpilot::utils::common::Logger>()) {}
  ~ConfiguratorManager() = default;

  ConfiguratorManager(const ConfiguratorManager&) = delete;
  ConfiguratorManager& operator=(const ConfiguratorManager&) = delete;

  struct Options {
    std::filesystem::path cfg_path;
  };

  enum class State : uint32_t {
    kPreInit,
    kInit,
    kStart,
    kShutdown,
  };

  const nxpilot::utils::common::Logger& GetLogger() const { return *logger_ptr_; }
  void SetLogger(const std::shared_ptr<nxpilot::utils::common::Logger> logger_ptr) {
    logger_ptr_ = logger_ptr;
  }

  void Initialize(const std::filesystem::path& cfg_file_path);
  void Start();
  void Shutdown();

  State GetState() const { return state_.load(); }

  YAML::Node GetRootOptionsNode() const { return root_options_node_; }
  YAML::Node GetNodeOptionsByKey(std::string_view key);

 private:
  std::shared_ptr<nxpilot::utils::common::Logger> logger_ptr_;
  Options options_;
  std::atomic<State> state_ = State::kPreInit;
  YAML::Node root_options_node_;
};

}  // namespace nxpilot::runtime::core::configurator