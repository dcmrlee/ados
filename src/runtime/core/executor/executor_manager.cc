// Copyright (C) 2024. All rights reserved.

#include <ranges>

#include "runtime/core/executor/executor_manager.h"
#include "runtime/core/executor/guard_thread_executor.h"
#include "runtime/core/executor/main_thread_executor.h"
#include "runtime/core/executor/time_wheel_executor.h"

namespace YAML {
template <>
struct convert<nxpilot::runtime::core::executor::ExecutorManager::Options> {
  using Options = nxpilot::runtime::core::executor::ExecutorManager::Options;

  static Node encode(const Options& rhs) {
    Node node;
    node["executors"] = YAML::Node();
    for (const auto& executor : rhs.executors_options) {
      Node executor_node;
      executor_node["name"] = executor.name;
      executor_node["type"] = executor.type;
      executor_node["options"] = executor.options;
      node["executors"].push_back(executor_node);
    }

    return node;
  }

  static bool decode(const Node& node, Options& rhs) {
    if (!node.IsMap()) {
      return false;
    }

    if (node["executors"] && node["executors"].IsSequence()) {
      for (const auto& executor_node : node["executors"]) {
        auto executor_options =
            Options::ExecutorOptions{.name = executor_node["name"].as<std::string>(),
                                     .type = executor_node["type"].as<std::string>()};

        if (executor_node["options"]) {
          executor_options.options = executor_node["options"];
        } else {
          executor_options.options = YAML::Node(YAML::NodeType::Null);
        }

        rhs.executors_options.emplace_back(std::move(executor_options));
      }
    }

    return true;
  }
};
}  // namespace YAML

namespace nxpilot::runtime::core::executor {

void ExecutorManager::Initialize(YAML::Node options_node) {
  NXPILOT_CHECK_ERROR(std::atomic_exchange(&state_, State::kInit) == State::kPreInit,
                      "ExecutorManager can only be initialized once.");

  if (options_node && !options_node.IsNull()) {
    options_ = options_node.as<Options>();
  }

  std::string default_main_thread_name = "nxpilot_main";
  YAML::Node default_main_thread_options = YAML::Node(YAML::NodeType::Null);
  std::string default_guard_thread_name = "nxpilot_guard";
  YAML::Node default_guard_thread_options = YAML::Node(YAML::NodeType::Null);

  // check options
  uint32_t idx = 0;
  bool detected_main_thread_options = false;
  for (auto& executor_options : options_.executors_options) {
    if (executor_options.type == "main_thread") {
      NXPILOT_CHECK_ERROR(idx == 0, "MainThreadExecutor should be the first");
      default_main_thread_name = executor_options.name;
      default_main_thread_options = executor_options.options;
      detected_main_thread_options = true;
    } else if (executor_options.type == "guard_thread") {
      NXPILOT_CHECK_ERROR(
          (idx == 0 && detected_main_thread_options == false) ||
              (idx == 1 && detected_main_thread_options == true),
          "GuardThreadExecutor should be the fisrt without MainThreadExecutor, or GuardThreadExecutor should be the second after MainThreadExecutor");
      default_guard_thread_name = executor_options.name;
      default_guard_thread_options = executor_options.options;
    }
    ++idx;
  }

  // First MainThreadExecutor
  {
    std::unique_ptr<ExecutorBase> executor_ptr = GetMainThreadExecutor();
    executor_ptr->Initialize(default_main_thread_name, default_main_thread_options);
    used_executor_names_.push_back(default_main_thread_name);
    executor_map_.emplace(default_main_thread_name, std::move(executor_ptr));
  }

  // Second GetGuardThreadExecutor
  {
    std::unique_ptr<ExecutorBase> executor_ptr = GetGuardThreadExecutor();
    executor_ptr->Initialize(default_guard_thread_name, default_guard_thread_options);
    used_executor_names_.push_back(default_guard_thread_name);
    executor_map_.emplace(default_guard_thread_name, std::move(executor_ptr));
  }

  for (auto& executor_options : options_.executors_options) {
    std::unique_ptr<ExecutorBase> executor_ptr;

    if (executor_options.type == "main_thread") {
      continue;
    } else if (executor_options.type == "guard_thread") {
      continue;
    } else {
      NXPILOT_CHECK_ERROR(executor_map_.find(executor_options.name) == executor_map_.end(),
                          "Duplicate executor name '{}'", executor_options.name);
      if (executor_options.type == "time_wheel") {
        executor_ptr = GetTimeWheelExecutor();
      } else {
        NXPILOT_CHECK_ERROR(false, "Invalid executor type '{}'", executor_options.type);
      }
    }

    executor_ptr->Initialize(executor_options.name, executor_options.options);
    used_executor_names_.push_back(executor_options.name);
    executor_map_.emplace(executor_options.name, std::move(executor_ptr));
  }

  NXPILOT_INFO("ExecutorManager init completed");
}

void ExecutorManager::Start() {
  NXPILOT_CHECK_ERROR(std::atomic_exchange(&state_, State::kStart) == State::kInit,
                      "Method can only be called when state is 'kInit'.");
  for (auto& executor_name : used_executor_names_) {
    auto iter = executor_map_.find(executor_name);
    NXPILOT_CHECK_ERROR(iter != executor_map_.end(), "Missing '{}' in executor_map_",
                        executor_name);
    iter->second->Start();
  }
}

void ExecutorManager::Shutdown() {
  if (std::atomic_exchange(&state_, State::kShutdown) == State::kShutdown) {
    return;
  }

  for (auto executor_name : std::views::reverse(used_executor_names_)) {
    auto iter = executor_map_.find(executor_name);
    NXPILOT_CHECK_ERROR(iter != executor_map_.end(), "Missing '{}' in executor_map_",
                        executor_name);
    iter->second->Shutdown();
  }

  NXPILOT_INFO("ExecutorManager shutdown");
}

std::unique_ptr<ExecutorBase> ExecutorManager::GetMainThreadExecutor() {
  NXPILOT_CHECK_ERROR(state_.load() == State::kInit,
                      "Method can only be called when state is 'kInit'.");
  auto ptr = std::make_unique<MainThreadExecutor>();
  ptr->SetLogger(logger_ptr_);
  return ptr;
}

std::unique_ptr<ExecutorBase> ExecutorManager::GetGuardThreadExecutor() {
  NXPILOT_CHECK_ERROR(state_.load() == State::kInit,
                      "Method can only be called when state is 'kInit'.");
  auto ptr = std::make_unique<GuardThreadExecutor>();
  ptr->SetLogger(logger_ptr_);
  return ptr;
}

std::unique_ptr<ExecutorBase> ExecutorManager::GetTimeWheelExecutor() {
  NXPILOT_CHECK_ERROR(state_.load() == State::kInit,
                      "Method can only be called when state is 'kInit'.");
  auto ptr = std::make_unique<TimeWheelExecutor>();
  ptr->SetLogger(logger_ptr_);
  return ptr;
}

}  // namespace nxpilot::runtime::core::executor