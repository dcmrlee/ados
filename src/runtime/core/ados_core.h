// Copyright (C) 2024. All rights reserved.

#pragma once

#include <future>
#include <string>
#include <vector>

#include "runtime/core/configurator/configurator_manager.h"
#include "runtime/core/executor/executor_manager.h"
#include "utils/common/log_tool.h"

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

    kPreShutdownPlugin,
    kPostShutdownPlugin,

    kPreShutdownConfigurator,
    kPostShutdownConfigurator,

    kPostShutdown,

    kMaxStateNum,
  };

  using HookTask = std::function<void()>;

  const nxpilot::utils::common::Logger& GetLogger() const { return *logger_ptr_; }

  void Initialize(const Options& options);
  void Start();
  void Shutdown();

  State GetState() const { return state_; }

 private:
  void EnterState(State state);
  void StartImpl();
  void ShutdownImpl();

 private:
  std::shared_ptr<nxpilot::utils::common::Logger> logger_ptr_;
  Options options_;
  State state_ = State::kPreInit;

  std::atomic_bool shutdown_flag_ = false;
  std::promise<void> shutdown_promise_;
  std::atomic_bool shutdown_impl_flag_ = false;

  std::vector<std::vector<HookTask>> hook_task_vec_array_;

  nxpilot::runtime::core::configurator::ConfiguratorManager configurator_manager_;
  nxpilot::runtime::core::executor::ExecutorManager executor_manager_;
};

}  // namespace nxpilot::runtime::core
