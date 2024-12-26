// Copyright (C) 2024. All rights reserved.

#include "runtime/core/ados_core.h"

namespace nxpilot::runtime::core {

AdosCore::AdosCore() : logger_ptr_(std::make_shared<nxpilot::utils::common::Logger>()) {
  NXPILOT_INFO("AdosCore constuctor");
  hook_task_vec_array_.resize(static_cast<uint32_t>(State::kMaxStateNum));
}

AdosCore::~AdosCore() {
  try {
    ShutdownImpl();
  } catch (const std::exception& e) {
    NXPILOT_INFO("AdosCore destruct get exception, {}", e.what());
  }
  hook_task_vec_array_.clear();
}

void AdosCore::Initialize(const Options& options) {
  EnterState(State::kPreInit);

  options_ = options;

  // Init configurator
  EnterState(State::kPreInitConfigurator);
  configurator_manager_.SetLogger(logger_ptr_);
  configurator_manager_.Initialize(options_.cfg_file_path);
  EnterState(State::kPostInitConfigurator);

  // Init Executor
  EnterState(State::kPreInitExecutor);
  executor_manager_.SetLogger(logger_ptr_);
  executor_manager_.Initialize(configurator_manager_.GetNodeOptionsByKey("executor"));
  EnterState(State::kPostInitExecutor);

  EnterState(State::kPostInit);
}

void AdosCore::Start() {
  StartImpl();
  NXPILOT_INFO("Nxpilot start completed, will waiting for shutdown.");
  shutdown_promise_.get_future().wait();

  ShutdownImpl();
}

void AdosCore::Shutdown() {
  if (std::atomic_exchange(&shutdown_flag_, true)) {
    return;
  }
  shutdown_promise_.set_value();
}

void AdosCore::EnterState(State state) {
  state_ = state;

  for (const auto& func : hook_task_vec_array_[static_cast<uint32_t>(state)]) {
    func();
  }
}

void AdosCore::StartImpl() {
  EnterState(State::kPreStart);

  EnterState(State::kPreStartConfigurator);
  configurator_manager_.Start();
  EnterState(State::kPostStartConfigurator);

  EnterState(State::kPreStartExecutor);
  executor_manager_.Start();
  EnterState(State::kPostStartExecutor);

  EnterState(State::kPostStart);
}

void AdosCore::ShutdownImpl() {
  if (std::atomic_exchange(&shutdown_impl_flag_, true)) return;

  EnterState(State::kPreShutdown);

  EnterState(State::kPreShutdownExecutor);
  executor_manager_.Shutdown();
  EnterState(State::kPostShutdownExecutor);

  EnterState(State::kPreShutdownConfigurator);
  configurator_manager_.Shutdown();
  EnterState(State::kPostShutdownConfigurator);

  EnterState(State::kPostShutdown);
}

}  // namespace nxpilot::runtime::core
