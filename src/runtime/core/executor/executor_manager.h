// Copyright (C) 2024. All rights reserved.

#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "runtime/core/executor/executor_base.h"
#include "utils/common/log_tool.h"
#include "utils/common/string_tool.h"
#include "yaml-cpp/yaml.h"

namespace nxpilot::runtime::core::executor {

class ExecutorManager {
 public:
  ExecutorManager() : logger_ptr_(std::make_shared<nxpilot::utils::common::Logger>()) {}
  ~ExecutorManager() = default;

  ExecutorManager(const ExecutorManager&) = delete;
  ExecutorManager& operator=(const ExecutorManager&) = delete;

  struct Options {
    struct ExecutorOptions {
      std::string name;
      std::string type;
      YAML::Node options;
    };
    std::vector<ExecutorOptions> executors_options;
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

  std::unique_ptr<ExecutorBase> GetExecutor(std::string_view executor_name);
  const std::vector<std::unique_ptr<ExecutorBase>>& GetAllExecutors() const;

 private:
  std::unique_ptr<ExecutorBase> GetMainThreadExecutor();
  std::unique_ptr<ExecutorBase> GetGuardThreadExecutor();
  std::unique_ptr<ExecutorBase> GetTimeWheelExecutor();

 private:
  std::shared_ptr<nxpilot::utils::common::Logger> logger_ptr_;
  Options options_;
  std::atomic<State> state_ = State::kPreInit;

  std::vector<std::string> used_executor_names_;
  std::unordered_map<std::string, std::unique_ptr<ExecutorBase>, nxpilot::utils::common::StringHash,
                     std::equal_to<>>
      executor_map_;
};

}  // namespace nxpilot::runtime::core::executor