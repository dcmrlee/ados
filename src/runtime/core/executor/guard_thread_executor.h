// Copyright (C) 2024. All rights reserved.

#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include "runtime/core/executor/executor_base.h"
#include "utils/common/log_util.h"
#include "yaml-cpp/yaml.h"

namespace nxpilot::runtime::core::executor {

class GuardThreadExecutor : public ExecutorBase {
 public:
  GuardThreadExecutor() : logger_ptr_(std::make_shared<nxpilot::utils::common::Logger>()) {}
  ~GuardThreadExecutor() = default;

  struct Options {
    std::string name = "nxpilot_guard";
    std::string thread_sched_policy;
    std::vector<uint32_t> thread_bind_cpu;
    uint32_t queue_threshold = 10000;
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

  std::string_view Type() const noexcept override { return type_; }
  std::string_view Name() const noexcept override { return options_.name; }

  bool ThreadSafe() const noexcept override { return true; }

  void Execute(Task&& task) noexcept override;

  bool SupportTimerSchedule() const noexcept override { return false; }
  std::chrono::system_clock::time_point Now() const noexcept override;
  void ExecuteAt(std::chrono::system_clock::time_point tp, Task&& task) noexcept override;

  size_t CurrentTaskNum() noexcept override { return queue_task_num_.load(); }

 private:
  std::shared_ptr<nxpilot::utils::common::Logger> logger_ptr_;
  Options options_;
  std::atomic<State> state_ = State::kPreInit;

  std::thread::id thread_id_;
  std::string_view type_ = "guard_thread";

  uint32_t queue_warn_threshold_;
  std::atomic_uint32_t queue_task_num_ = 0;
  std::mutex mutex_;
  std::condition_variable cond_;
  std::queue<Task> queue_;
  std::unique_ptr<std::thread> thread_ptr_;
};

}  // namespace nxpilot::runtime::core::executor