// Copyright (C) 2024. All rights reserved.

#include <string>

#include "gtest/gtest.h"

#include "utils/common/thread_tool.h"

namespace nxpilot::utils::common {

std::string GetCurrentThreadName() {
  char name[16];
  pthread_getname_np(pthread_self(), name, 16);
  return std::string(name);
}

std::vector<uint32_t> GetCurrentThreadAffinity() {
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);

  if (pthread_getaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0) {
    std::cerr << "Failed to get thread affinity" << std::endl;
  }

  std::vector<uint32_t> cpu_indices;
  for (int i = 0; i < CPU_SETSIZE; ++i) {
    if (CPU_ISSET(i, &cpuset)) {
      cpu_indices.push_back(i);
    }
  }
  return cpu_indices;
}

TEST(ThreadToolTest, SetNameForCurrentThread) {
  std::string short_name = "short_name";
  ThreadTool::SetNameForCurrentThread(short_name);
  EXPECT_EQ(GetCurrentThreadName(), short_name);

  std::string long_name = "long_thread_name_more_than_15_characters_long";
  ThreadTool::SetNameForCurrentThread(long_name);
  std::string expected_name = "long_thr.._long";
  EXPECT_EQ(GetCurrentThreadName(), expected_name);
}

TEST(ThreadToolTest, BindCpuForCurrentThread) {
  std::vector<uint32_t> cpu_set = {0, 1};
  EXPECT_NO_THROW(ThreadTool::BindCpuForCurrentThread(cpu_set));
  EXPECT_EQ(cpu_set, GetCurrentThreadAffinity());

  std::vector<uint32_t> empty_cpu_set = {};
  EXPECT_NO_THROW(ThreadTool::BindCpuForCurrentThread(empty_cpu_set));

  EXPECT_ANY_THROW(ThreadTool::BindCpuForCurrentThread({10000}));
}

// Need Root Permission
TEST(ThreadToolTest, SetCpuSchedForCurrentThread) {
  EXPECT_NO_THROW(ThreadTool::SetCpuSchedForCurrentThread(""));
  EXPECT_NO_THROW(ThreadTool::SetCpuSchedForCurrentThread("SCHED_OTHER"));
  EXPECT_NO_THROW(ThreadTool::SetCpuSchedForCurrentThread("SCHED_FIFO:50"));
  EXPECT_NO_THROW(ThreadTool::SetCpuSchedForCurrentThread("SCHED_RR:80"));

  EXPECT_ANY_THROW(ThreadTool::SetCpuSchedForCurrentThread("SCHED_INVALID"));
  EXPECT_ANY_THROW(ThreadTool::SetCpuSchedForCurrentThread("SCHED_FIFO:99999"));

  EXPECT_THROW(ThreadTool::SetCpuSchedForCurrentThread("SCHED_FIFO"),
               nxpilot::utils::common::NxpilotException);
  EXPECT_THROW(ThreadTool::SetCpuSchedForCurrentThread("SCHED_FIFO:"),
               nxpilot::utils::common::NxpilotException);
}

}  // namespace nxpilot::utils::common