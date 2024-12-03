#include <iostream>

#include "gflags/gflags.h"

DEFINE_bool(h, false, "help");
DEFINE_bool(v, false, "version");

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
  return 0;
}
