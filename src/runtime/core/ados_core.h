// Copyright (C) 2024. All rights reserved.

#pragma once

#include "utils/common/block_queue.h"
#include "utils/common/log_util.h"

namespace nxpilot::runtime::core {

class AdosCore {
 public:
  AdosCore();
  ~AdosCore();
  AdosCore(const AdosCore&) = delete;
  AdosCore& operator=(const AdosCore&) = delete;

  const auto& GetLogger() const { return *logger_ptr_; }

 private:
  nxpilot::utils::common::BlockQueue<int> queue_;
  std::shared_ptr<nxpilot::utils::common::Logger> logger_ptr_;
};

}  // namespace nxpilot::runtime::core
