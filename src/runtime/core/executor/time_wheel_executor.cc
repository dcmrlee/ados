// Copyright (C) 2024. All rights reserved.

#include "runtime/core/executor/time_wheel_executor.h"

namespace YAML {
template <>
struct convert<nxpilot::runtime::core::executor::TimeWheelExecutor::Options> {
  using Options = nxpilot::runtime::core::executor::TimeWheelExecutor::Options;

  static Node encode(const Options& rhs) {
    Node node;

    node["bind_executor"] = rhs.bind_executor;
    node["thread_sched_policy"] = rhs.thread_sched_policy;
    node["thread_bind_cpu"] = rhs.thread_bind_cpu;
    node["dt_us"] = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(rhs.dt).count());
    node["wheel_size"] = rhs.wheel_size;

    return node;
  }

  static bool decode(const Node& node, Options& rhs) {
    if (!node.IsMap()) return false;

    if (node["bind_executor"]) rhs.bind_executor = node["bind_executor"].as<std::string>();
    if (node["thread_sched_policy"])
      rhs.thread_sched_policy = node["thread_sched_policy"].as<std::string>();
    if (node["thread_bind_cpu"])
      rhs.thread_bind_cpu = node["thread_bind_cpu"].as<std::vector<uint32_t>>();
    if (node["dt_us"]) rhs.dt = std::chrono::microseconds(node["dt_us"].as<uint64_t>());
    if (node["wheel_size"]) rhs.wheel_size = node["wheel_size"].as<std::vector<size_t>>();

    return true;
  }
};
}  // namespace YAML

namespace nxpilot::runtime::core::executor {

void TimeWheelExecutor::Initialize(std::string_view name, YAML::Node options_node) {
  NXPILOT_CHECK_ERROR(std::atomic_exchange(&state_, State::kInit) == State::kPreInit,
                      "TimeWheelExecutor can only be initialized once.");
  name_ = std::string(name);
  if (options_node && !options_node.IsNull()) {
    options_ = options_node.as<Options>();
  }
  NXPILOT_INFO("TimeWheelExecutor init completed");
}

void TimeWheelExecutor::Start() {
  NXPILOT_CHECK_ERROR(std::atomic_exchange(&state_, State::kStart) == State::kInit,
                      "TimeWheelExecutor can only run when state is 'Init'.");
  NXPILOT_INFO("TimeWheelExecutor start completed");
}

void TimeWheelExecutor::Shutdown() {
  if (std::atomic_exchange(&state_, State::kShutdown) == State::kShutdown) {
    return;
  }

  NXPILOT_INFO("TimeWheelExecutor shutdown");
}

void TimeWheelExecutor::Execute(Task&& task) noexcept {
  NXPILOT_ERROR("TimeWheelExecutor does not impl execute");
}

std::chrono::system_clock::time_point TimeWheelExecutor::Now() const noexcept {
  NXPILOT_ERROR("TimeWheelExecutor does not support timer schedule");
  return std::chrono::system_clock::time_point();
}

void TimeWheelExecutor::ExecuteAt(std::chrono::system_clock::time_point tp, Task&& task) noexcept {
  NXPILOT_ERROR("TimeWheelExecutor does not support timer schedule");
}

}  // namespace nxpilot::runtime::core::executor