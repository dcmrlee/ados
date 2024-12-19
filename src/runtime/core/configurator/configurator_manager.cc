// Copyright (C) 2024. All rights reserved.

#include "runtime/core/configurator/configurator_manager.h"

namespace YAML {
template <>
struct convert<nxpilot::runtime::core::configurator::ConfiguratorManager::Options> {
  using Options = nxpilot::runtime::core::configurator::ConfiguratorManager::Options;

  static Node encode(const Options& rhs) {
    Node node;
    node["cfg_path"] = rhs.cfg_path.string();
    return node;
  }

  static bool decode(const Node& node, Options& rhs) {
    if (!node.IsMap()) {
      return false;
    }

    if (node["cfg_path"]) {
      rhs.cfg_path = node["cfg_path"].as<std::string>();
    } else {
      return false;
    }
  }
};
}  // namespace YAML

namespace nxpilot::runtime::core::configurator {

void ConfiguratorManager::Initialize(const std::filesystem::path& cfg_file_path) {
  NXPILOT_CHECK_ERROR(std::atomic_exchange(&state_, State::kInit) == State::kPreInit,
                      "Configurator manager can only be initialized once.");
  NXPILOT_CHECK_ERROR(!cfg_file_path.empty(), "Nxpilot start with no cfg file.");

  options_.cfg_path = std::filesystem::canonical(std::filesystem::absolute(cfg_file_path));

  root_options_node_ = YAML::LoadFile(options_.cfg_path.string());

  NXPILOT_INFO("ConfiguratorManager init completed");
}

void ConfiguratorManager::Start() {
  NXPILOT_CHECK_ERROR(std::atomic_exchange(&state_, State::kStart) == State::kInit,
                      "Method can only be called when state is 'Init'.");
  NXPILOT_INFO("ConfiguratorManager start completed");
}

void ConfiguratorManager::Shutdown() {
  if (std::atomic_exchange(&state_, State::kShutdown) == State::kShutdown) {
    return;
  }
  NXPILOT_INFO("ConfiguratorManager shutdown");
}

YAML::Node ConfiguratorManager::GetNodeOptionsByKey(std::string_view key) {
  NXPILOT_CHECK_ERROR(state_.load() == State::kInit,
                      "Method can only be called when state is 'Init'.");
  return root_options_node_[key];
}

}  // namespace nxpilot::runtime::core::configurator