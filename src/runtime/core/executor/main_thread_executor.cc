// Copyright (C) 2024. All rights reserved.

#include "runtime/core/executor/main_thread_executor.h"
#include "utils/common/thread_tool.h"

namespace YAML {
template <>
struct convert<nxpilot::runtime::core::executor::MainThreadExecutor::Options> {
  using Options = nxpilot::runtime::core::executor::MainThreadExecutor::Options;

  static Node encode(const Options& rhs) {
    Node node;
    node["thread_sched_policy"] = rhs.thread_sched_policy;
    node["thread_bind_cpu"] = rhs.thread_bind_cpu;

    return node;
  }

  static bool decode(const Node& node, Options& rhs) {
    if (!node.IsMap()) {
      return false;
    }

    if (node["thread_sched_policy"]) {
      rhs.thread_sched_policy = node["thread_sched_policy"].as<std::string>();
    }

    if (node["thread_bind_cpu"]) {
      rhs.thread_bind_cpu = node["thread_bind_cpu"].as<std::vector<uint32_t>>();
    }

    return true;
  }
};
}  // namespace YAML

namespace nxpilot::runtime::core::executor {

void MainThreadExecutor::Initialize(std::string_view name, YAML::Node options_node) {
  NXPILOT_CHECK_ERROR(std::atomic_exchange(&state_, State::kInit) == State::kPreInit,
                      "MainThreadExecutor can only be initialized once.");
  name_ = std::string(name);

  if (options_node && !options_node.IsNull()) {
    options_ = options_node.as<Options>();
  }

  try {
    nxpilot::utils::common::SetNameForCurrentThread(name_);
    nxpilot::utils::common::BindCpuForCurrentThread(options_.thread_bind_cpu);
    nxpilot::utils::common::SetCpuSchedForCurrentThread(options_.thread_sched_policy);
  } catch (const std::exception& e) {
    NXPILOT_ERROR("Set thread policy for MainThreadExecutor get exception, {}", e.what());
  }

  main_thread_id_ = std::this_thread::get_id();

  NXPILOT_INFO("MainThreadExecutor init completed");
}

void MainThreadExecutor::Start() {
  NXPILOT_CHECK_ERROR(std::atomic_exchange(&state_, State::kStart) == State::kInit,
                      "MainThreadExecutor can only run when state is 'Init'.");
  NXPILOT_INFO("MainThreadExecutor start completed");
}

void MainThreadExecutor::Shutdown() {
  if (std::atomic_exchange(&state_, State::kShutdown) == State::kShutdown) {
    return;
  }

  NXPILOT_INFO("MainThreadExecutor shutdown");
}

void MainThreadExecutor::Execute(Task&& task) noexcept {
  NXPILOT_ERROR("MainThreadExecutor does not impl execute");
}

std::chrono::system_clock::time_point MainThreadExecutor::Now() const noexcept {
  NXPILOT_ERROR("MainThreadExecutor does not support timer schedule");
  return std::chrono::system_clock::time_point();
}

void MainThreadExecutor::ExecuteAt(std::chrono::system_clock::time_point tp, Task&& task) noexcept {
  NXPILOT_ERROR("MainThreadExecutor does not support timer schedule");
}

}  // namespace nxpilot::runtime::core::executor