// Copyright (C) 2024. All rights reserved.

#pragma once

#include <atomic>
#include <functional>
#include <shared_mutex>
#include <thread>
#include <memory>
#include <mutex>

#include "runtime/core/executor/executor_base.h"
#include "utils/common/log_tool.h"
#include "yaml-cpp/yaml.h"

namespace nxpilot::runtime::core::executor {

class TimeWheelExecutor : public ExecutorBase {
 public:
  TimeWheelExecutor() : logger_ptr_(std::make_shared<nxpilot::utils::common::Logger>()) {}
  ~TimeWheelExecutor() = default;

  struct Options {
    std::string bind_executor;
    std::string thread_sched_policy;
    std::vector<uint32_t> thread_bind_cpu;
    std::chrono::nanoseconds dt = std::chrono::microseconds(1000);
    std::vector<size_t> wheel_size = {1000, 600};
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

  bool SupportTimerSchedule() const noexcept override { return true; }
  std::chrono::system_clock::time_point Now() const noexcept override;
  void ExecuteAt(std::chrono::system_clock::time_point tp, Task&& task) noexcept override;

  size_t CurrentTaskNum() noexcept override { return 1; }

 private:
  void TimerLoop();
  struct TaskWithTimestamp {
    uint64_t tick_count;  // 距离start_time的时间tick
    Task task;
  };

  using TaskList = std::list<TaskWithTimestamp>;

  struct TimingWheelTool {
    uint64_t current_pos;
    uint64_t scale;
    std::vector<TaskList> wheel;
    std::function<void()> borrow_func;

    TaskList Tick() {
      TaskList task_list;
      task_list.swap(wheel[current_pos]);

      ++current_pos;
      if (current_pos == wheel.size()) [[unlikely]] {
        current_pos = 0;
        borrow_func();
      }

      return task_list;
    }
  };

 private:
  std::shared_ptr<nxpilot::utils::common::Logger> logger_ptr_;
  Options options_;
  std::atomic<State> state_ = State::kPreInit;

  std::string name_;
  std::thread::id thread_id_;
  std::string_view type_ = "time_wheel";

  uint64_t dt_count_;
  std::vector<TimingWheelTool> timing_wheel_vec_;  // 多级时间轮
  uint64_t timing_task_map_pos_ = 0;
  std::map<uint64_t, TaskList> timing_task_map_;

  std::unique_ptr<std::thread> timer_thread_ptr_;
  std::atomic_bool start_flag_ = false;
  uint64_t start_time_point_ = 0;

  mutable std::shared_mutex tick_mutex_;
  uint64_t current_tick_count_ = 0;
};

}  // namespace nxpilot::runtime::core::executor