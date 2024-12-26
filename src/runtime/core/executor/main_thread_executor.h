// Copyright (C) 2024. All rights reserved.

#pragma once

#include <atomic>
#include <functional>
#include <thread>

#include "runtime/core/executor/executor_base.h"
#include "utils/common/log_tool.h"
#include "yaml-cpp/yaml.h"

namespace nxpilot::runtime::core::executor {

class MainThreadExecutor : public ExecutorBase {
 public:
  MainThreadExecutor() : logger_ptr_(std::make_shared<nxpilot::utils::common::Logger>()) {}
  ~MainThreadExecutor() = default;

  struct Options {
    std::string name = "nxpilot_main";
    std::string thread_sched_policy;
    std::vector<uint32_t> thread_bind_cpu;
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

  void Initialize(std::string_view name, YAML::Node options_node) override;
  void Start() override;
  void Shutdown() override;

  State GetState() const { return state_.load(); }

  std::string_view Type() const noexcept override { return type_; }
  std::string_view Name() const noexcept override { return name_; }

  bool ThreadSafe() const noexcept override { return true; }

  void Execute(Task&& task) noexcept override;

  bool SupportTimerSchedule() const noexcept override { return false; }
  std::chrono::system_clock::time_point Now() const noexcept override;
  void ExecuteAt(std::chrono::system_clock::time_point tp, Task&& task) noexcept override;

  size_t CurrentTaskNum() noexcept override { return 1; }

 private:
  std::shared_ptr<nxpilot::utils::common::Logger> logger_ptr_;
  Options options_;
  std::atomic<State> state_ = State::kPreInit;

  std::string name_;
  std::thread::id main_thread_id_;
  std::string_view type_ = "main_thread";
};

}  // namespace nxpilot::runtime::core::executor