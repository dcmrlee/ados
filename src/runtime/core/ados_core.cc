// Copyright (C) 2024. All rights reserved.

#include "runtime/core/ados_core.h"

namespace nxpilot::runtime::core {

AdosCore::AdosCore() : logger_ptr_(std::make_shared<nxpilot::utils::common::Logger>()) {
  NXPILOT_INFO("AdosCore constuctor");
  hook_task_vec_array_.resize(static_cast<uint32_t>(State::kMaxStateNum));
}

AdosCore::~AdosCore() {}

}  // namespace nxpilot::runtime::core
