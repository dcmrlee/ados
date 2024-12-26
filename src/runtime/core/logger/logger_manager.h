// Copyright (C) 2024. All rights reserved.

#pragma once

#include <atomic>

#include "runtime/core/executor/guard_thread_executor.h"
#include "utils/common/log_tool.h"
#include "yaml-cpp/yaml.h"

namespace nxpilot::runtime::core::logger {

class LoggerManager {
 public:
  LoggerManager() : logger_ptr_(std::make_shared<nxpilot::utils::common::Logger>()) {}
  ~LoggerManager() = default;

  struct Options {
    struct BackendOptions {
      std::string type;
      YAML::Node options;
    };

    std::vector<BackendOptions> backends_options;
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

  void Initialize(YAML::Node options_node);
  void Start();
  void Shutdown();

  State GetState() const { return state_.load(); }

 private:
  std::shared_ptr<nxpilot::utils::common::Logger> logger_ptr_;
  Options options_;
  std::atomic<State> state_ = State::kPreInit;
};

}  // namespace nxpilot::runtime::core::logger