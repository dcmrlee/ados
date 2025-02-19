// Copyright (C) 2024. All rights reserved.

#include <csignal>
#include <iostream>

#include "gflags/gflags.h"

#include "runtime/core/ados_core.h"

DEFINE_string(cfg_file_path, "", "config file path");

DEFINE_bool(h, false, "help");
DEFINE_bool(v, false, "version");

nxpilot::runtime::core::AdosCore* global_core_ptr = nullptr;

void SignalHandler(int sig) {
  if (global_core_ptr && (sig == SIGINT || sig == SIGTERM)) {
    global_core_ptr->Shutdown();
    return;
  }
  raise(sig);
}

void PrintVersion() { std::cout << "Version" << std::endl; }

void PrintUsage() { std::cout << "Usage" << std::endl; }

void ParseCommandLineFlags(int32_t argc, char** argv) {
  if (argc == 1) {
    PrintUsage();
    exit(0);
  }

  gflags::ParseCommandLineNonHelpFlags(&argc, &argv, true);

  if (FLAGS_h) {
    PrintUsage();
    exit(0);
  }

  if (FLAGS_v) {
    PrintVersion();
    exit(0);
  }
}

int32_t main(int32_t argc, char** argv) {
  ParseCommandLineFlags(argc, argv);
  signal(SIGINT, SignalHandler);
  signal(SIGTERM, SignalHandler);

  std::cout << "NXpilot start!" << std::endl;
  try {
    nxpilot::runtime::core::AdosCore core;
    global_core_ptr = &core;

    nxpilot::runtime::core::AdosCore::Options options{.cfg_file_path = FLAGS_cfg_file_path};
    core.Initialize(options);
    core.Start();
    core.Shutdown();
    global_core_ptr = nullptr;
  } catch (const std::exception& e) {
    std::cout << "NXpilot run with exception and exit. " << e.what() << std::endl;
    return -1;
  }
  std::cout << "NXpilot exit!" << std::endl;
  return 0;
}
