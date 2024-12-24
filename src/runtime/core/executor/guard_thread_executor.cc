// Copyright (C) 2024. All rights reserved.

#include "runtime/core/executor/guard_thread_executor.h"
#include "utils/common/thread_tool.h"

namespace YAML {
template <>
struct convert<nxpilot::runtime::core::executor::GuardThreadExecutor::Options> {
  using Options = nxpilot::runtime::core::executor::GuardThreadExecutor::Options;

  static Node encode(const Options& rhs) {
    Node node;
    node["name"] = rhs.name;
    node["thread_sched_policy"] = rhs.thread_sched_policy;
    node["thread_bind_cpu"] = rhs.thread_bind_cpu;
    node["queue_threshold"] = rhs.queue_threshold;

    return node;
  }

  static bool decode(const Node& node, Options& rhs) {
    if (!node.IsMap()) {
      return false;
    }

    if (node["name"]) {
      rhs.name = node["name"].as<std::string>();
    }

    if (node["thread_sched_policy"]) {
      rhs.thread_sched_policy = node["thread_sched_policy"].as<std::string>();
    }

    if (node["thread_bind_cpu"]) {
      rhs.thread_bind_cpu = node["thread_bind_cpu"].as<std::vector<uint32_t>>();
    }

    if (node["queue_threshold"]) {
      rhs.queue_threshold = node["queue_threshold"].as<uint32_t>();
    }

    return true;
  }
};
}  // namespace YAML

namespace nxpilot::runtime::core::executor {

void GuardThreadExecutor::Initialize(YAML::Node options_node) {
  NXPILOT_CHECK_ERROR(std::atomic_exchange(&state_, State::kInit) == State::kPreInit,
                      "GuardThreadExecutor can only be initialized once.");
  if (options_node && !options_node.IsNull()) {
    options_ = options_node.as<Options>();
  }

  queue_warn_threshold_ = options_.queue_threshold * 0.95;

  thread_ptr_ = std::make_unique<std::thread>([this]() {
    thread_id_ = std::this_thread::get_id();

    try {
      nxpilot::utils::common::ThreadTool::SetNameForCurrentThread(options_.name);
      nxpilot::utils::common::ThreadTool::BindCpuForCurrentThread(options_.thread_bind_cpu);
      nxpilot::utils::common::ThreadTool::SetCpuSchedForCurrentThread(options_.thread_sched_policy);
    } catch (const std::exception& e) {
      NXPILOT_ERROR("Set thread policy for guard thread get exception, {}", e.what());
    }

    while (state_.load() != State::kShutdown) {
      std::queue<Task> tmp_queue;
      {
        std::unique_lock<std::mutex> lck(mutex_);
        cond_.wait(lck, [this] { return !queue_.empty() || state_.load() == State::kShutdown; });
        queue_.swap(tmp_queue);
      }

      while (!tmp_queue.empty()) {
        auto& task = tmp_queue.front();
        try {
          task();
          --queue_task_num_;
        } catch (const std::exception& e) {
          NXPILOT_FATAL("Guard thread executor run task get exception, {}", e.what());
        }

        tmp_queue.pop();
      }
    }

    // After Shutdown, Run all the left task without lock.
    while (!queue_.empty()) {
      auto& task = queue_.front();
      try {
        task();
        --queue_task_num_;
      } catch (const std::exception& e) {
        NXPILOT_FATAL("Guard thread executor run task get exception, {}", e.what());
      }
      queue_.pop();
    }
  });

  NXPILOT_INFO("GuardThreadExecutor init completed");
}

void GuardThreadExecutor::Start() {
  NXPILOT_CHECK_ERROR(std::atomic_exchange(&state_, State::kStart) == State::kInit,
                      "GuardThreadExecutor can only run when state is 'Init'.");
  NXPILOT_INFO("GuardThreadExecutor start completed");
}

void GuardThreadExecutor::Shutdown() {
  if (std::atomic_exchange(&state_, State::kShutdown) == State::kShutdown) {
    return;
  }

  {
    std::unique_lock<std::mutex> lck(mutex_);
    cond_.notify_one();
  }

  if (thread_ptr_ && thread_ptr_->joinable()) {
    thread_ptr_->join();
  }

  thread_ptr_.reset();

  NXPILOT_INFO("GuardThreadExecutor shutdown");
}

void GuardThreadExecutor::Execute(Task&& task) noexcept {
  if (state_.load() != State::kStart) [[unlikely]] {
    NXPILOT_ERROR("Guard thread executor can only execute task when state is 'Start'.");
  }

  uint32_t cur_queue_task_num = ++queue_task_num_;

  if (cur_queue_task_num > options_.queue_threshold) [[unlikely]] {
    NXPILOT_ERROR(
        "The number of tasks in the guard thread executor has reached the threshold {}, the task will not be delivered.",
        options_.queue_threshold);
    --queue_task_num_;
    return;
  }

  if (cur_queue_task_num > queue_warn_threshold_) [[unlikely]] {
    NXPILOT_WARN(
        "The number of tasks in the guard thread executor is about to reached the threshold {}/{}",
        cur_queue_task_num, options_.queue_threshold);
  }

  std::unique_lock<std::mutex> lck(mutex_);
  queue_.emplace(std::move(task));
  cond_.notify_one();
}

std::chrono::system_clock::time_point GuardThreadExecutor::Now() const noexcept {
  NXPILOT_ERROR("Guard thread executor does not support timer schedule");
  return std::chrono::system_clock::time_point();
}

void GuardThreadExecutor::ExecuteAt(std::chrono::system_clock::time_point tp,
                                    Task&& task) noexcept {
  NXPILOT_ERROR("Guard thread executor does not support timer schedule");
}

}  // namespace nxpilot::runtime::core::executor
