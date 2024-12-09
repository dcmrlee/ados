#include <csignal>
#include <iostream>

#include "gflags/gflags.h"

DEFINE_bool(h, false, "help");
DEFINE_bool(v, false, "version");

void SignalHandler(int sig)
{
  if (sig == SIGINT || sig == SIGTERM) {
    std::cout << "Capture sig, Quit!" << std::endl;
    return;
  }
  raise(sig);
}

void PrintVersion()
{
  std::cout << "Version" << std::endl;
}

void PrintUsage()
{
  std::cout << "Usage" << std::endl;
}

void ParseCommandLineFlags(int32_t argc, char** argv)
{
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


int32_t main(int32_t argc, char** argv)
{
  ParseCommandLineFlags(argc, argv);
  signal(SIGINT, SignalHandler);
  signal(SIGTERM, SignalHandler);

  std::cout << "Mipilot start!" << std::endl;
  std::cout << "Mipilot exit!" << std::endl;
  return 0;
}
