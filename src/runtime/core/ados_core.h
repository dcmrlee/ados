// Copyright (C) 2024. All rights reserved.

#pragma once

#include "utils/common/block_queue.h"

namespace nxpilot::runtime::core {

class AdosCore {
 private:
  nxpilot::utils::common::BlockQueue<int> queue_;
};

}  // namespace nxpilot::runtime::core
