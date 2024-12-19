// Copyright (C) 2024. All rights reserved.

#include "runtime/core/ados_core.h"

namespace nxpilot::runtime::core {

AdosCore::AdosCore() : logger_ptr_(std::make_shared<nxpilot::utils::common::Logger>()) {
  NXPILOT_INFO("AdosCore constuctor");
  hook_task_vec_array_.resize(static_cast<uint32_t>(State::kMaxStateNum));
}

AdosCore::~AdosCore() { hook_task_vec_array_.clear(); }

void AdosCore::Initialize(const Options& options) {
  EnterState(State::kPreInit);

  options_ = options;

  // Init configurator
  EnterState(State::kPreInitConfigurator);
  configurator_manager_.SetLogger(logger_ptr_);
  configurator_manager_.Initialize(options_.cfg_file_path);
  EnterState(State::kPostInitConfigurator);
}

void AdosCore::EnterState(State state) {
  state_ = state;

  for (const auto& func : hook_task_vec_array_[static_cast<uint32_t>(state)]) {
    func();
  }
}

}  // namespace nxpilot::runtime::core
