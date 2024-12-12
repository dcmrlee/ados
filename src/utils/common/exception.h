// Copyright (C) 2024. All rights reserved.

#pragma once

#include <string>

namespace nxpilot::utils::common {
class NxpilotException : public std::exception {
 public:
  template <typename... Args>
    requires std::constructible_from<std::string, Args...>
  NxpilotException(Args... args) : err_msg_(std::forward<Args>(args)...) {}

  ~NxpilotException() noexcept override {}

  const char* what() const noexcept override { return err_msg_.c_str(); }

 private:
  std::string err_msg_;
};
}  // namespace nxpilot::utils::common

#define AIMRT_ASSERT(__expr__, __fmt__, ...)                                               \
  do {                                                                                     \
    if (!(__expr__)) [[unlikely]] {                                                        \
      throw nxpilot::utils::common::NxpilotException(std::format(__fmt__, ##__VA_ARGS__)); \
    }                                                                                      \
  } while (0)
