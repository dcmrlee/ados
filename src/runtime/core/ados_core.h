// Copyright (C) 2024. All rights reserved.

#pragma once

#include <string>
#include <vector>

#include "utils/common/block_queue.h"
#include "utils/common/log_util.h"
#include "runtime/core/configurator/configurator_manager.h"

namespace nxpilot::runtime::core {

class AdosCore {
 public:
  AdosCore();
  ~AdosCore();

  AdosCore(const AdosCore&) = delete;
  AdosCore& operator=(const AdosCore&) = delete;

  struct Options {
    std::string cfg_file_path;
  };

  enum class State : uint32_t {
    kPreInit,

    kPreInitConfigurator,
    kPostInitConfigurator,

    kPreInitPlugin,
    kPostInitPlugin,

    kPreInitMainThread,
    kPostInitMainThread,

    kPreInitGuardThread,
    kPostInitGuardThread,

    kPreInitExecutor,
    kPostInitExecutor,

    kPreInitLog,
    kPostInitLog,

    kPreInitAllocator,
    kPostInitAllocator,

    kPreInitRpc,
    kPostInitRpc,

    kPreInitChannel,
    kPostInitChannel,

    kPreInitParameter,
    kPostInitParameter,

    kPreInitModules,
    kPostInitModules,

    kPostInit,

    kPreStart,

    kPreStartConfigurator,
    kPostStartConfigurator,

    kPreStartPlugin,
    kPostStartPlugin,

    kPreStartMainThread,
    kPostStartMainThread,

    kPreStartGuardThread,
    kPostStartGuardThread,

    kPreStartExecutor,
    kPostStartExecutor,

    kPreStartLog,
    kPostStartLog,

    kPreStartAllocator,
    kPostStartAllocator,

    kPreStartRpc,
    kPostStartRpc,

    kPreStartChannel,
    kPostStartChannel,

    kPreStartParameter,
    kPostStartParameter,

    kPreStartModules,
    kPostStartModules,

    kPostStart,

    kPreShutdown,

    kPreShutdownModules,
    kPostShutdownModules,

    kPreShutdownParameter,
    kPostShutdownParameter,

    kPreShutdownChannel,
    kPostShutdownChannel,

    kPreShutdownRpc,
    kPostShutdownRpc,

    kPreShutdownAllocator,
    kPostShutdownAllocator,

    kPreShutdownLog,
    kPostShutdownLog,

    kPreShutdownExecutor,
    kPostShutdownExecutor,

    kPreShutdownGuardThread,
    kPostShutdownGuardThread,

    kPreShutdownMainThread,
    kPostShutdownMainThread,

    kPreShutdownPlugin,
    kPostShutdownPlugin,

    kPreShutdownConfigurator,
    kPostShutdownConfigurator,

    kPostShutdown,

    kMaxStateNum,
  };

  using HookTask = std::function<void()>;

  const nxpilot::utils::common::Logger& GetLogger() const { return *logger_ptr_; }

 private:
  nxpilot::utils::common::BlockQueue<int> queue_;
  std::shared_ptr<nxpilot::utils::common::Logger> logger_ptr_;
  std::vector<std::vector<HookTask>> hook_task_vec_array_;
};

}  // namespace nxpilot::runtime::core
