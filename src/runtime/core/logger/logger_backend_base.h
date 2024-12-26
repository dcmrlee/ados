// Copyright (C) 2024. All rights reserved.

#pragma once

#include "yaml-cpp/yaml.h"

namespace nxpilot::runtime::core::logger {

class LoggerBackendBase {
 public:
  LoggerBackendBase() = default;
  virtual ~LoggerBackendBase() = default;

  LoggerBackendBase(const LoggerBackendBase&) = delete;
  LoggerBackendBase& operator=(const LoggerBackendBase&) = delete;

  virtual std::string_view Type() const noexcept = 0;

  virtual void Initialize(YAML::Node options_node) = 0;
  virtual void Start() = 0;
  virtual void Shutdown() = 0;

  // This method will only be called after 'Initialize' and before 'Shutdown'.
  virtual void Log(uint32_t lvl, uint32_t line, uint32_t column, const char* file_name,
                   const char* function_name, const char* log_data,
                   size_t log_data_size) noexcept = 0;
};

}  // namespace nxpilot::runtime::core::logger