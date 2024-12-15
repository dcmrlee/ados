// Copyright (C) 2024. All rights reserved.

#pragma once

#include "yaml-cpp/yaml.h"
#include "utils/common/log_util.h"

namespace nxpilot::runtime::core::configurator {

class ConfiguratorManager {
 public:
  ConfiguratorManager() : logger_ptr_(std::make_shared<nxpilot::utils::common::Logger>()) {}
  ~ConfiguratorManager() = default;

  ConfiguratorManager(const ConfiguratorManager&) = delete;
  ConfiguratorManager& operator=(const ConfiguratorManager&) = delete;

  const nxpilot::utils::common::Logger& GetLogger() const { return *logger_ptr_; }

 private:
  std::shared_ptr<nxpilot::utils::common::Logger> logger_ptr_;
};

}  // namespace nxpilot::runtime::core::configurator