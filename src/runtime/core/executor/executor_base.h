// Copyright (C) 2024. All rights reserved.

#pragma once

#include <chrono>
#include <string>

#include "yaml-cpp/yaml.h"

namespace nxpilot::runtime::core::executor {

class ExecutorBase {
 public:
  using Task = std::function<void(void*)>;
  ExecutorBase() = default;
  virtual ~ExecutorBase() = default;
  ExecutorBase(const ExecutorBase&) = delete;
  ExecutorBase& operator=(const ExecutorBase&) = delete;

  virtual std::string_view Type() const noexcept = 0;
  virtual std::string_view Name() const noexcept = 0;

  virtual bool ThreadSafe() const noexcept = 0;

  virtual void Execute(Task&& task) noexcept = 0;

  virtual bool SupportTimerSchedule() const noexcept = 0;
  virtual std::chrono::system_clock::time_point Now() const noexcept = 0;
  virtual void ExecuteAt(std::chrono::system_clock::time_point tp, Task&& task) noexcept = 0;

  virtual size_t CurrentTaskNum() noexcept { return 0; }
};

}  // namespace nxpilot::runtime::core::executor