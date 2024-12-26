// Copyright (C) 2024. All rights reserved.

#pragma once

#include <cstdlib>
#include <string_view>
#include <vector>

#include "utils/common/exception.h"

namespace nxpilot::utils::common {

inline void SetNameForCurrentThread(std::string_view thread_name) {
  std::string name(thread_name);

  if (name.size() < 15) {
    pthread_setname_np(pthread_self(), name.data());
  } else {
    std::string real_name = name.substr(0, 8) + ".." + name.substr(name.size() - 5, name.size());
    pthread_setname_np(pthread_self(), real_name.c_str());
  }
}

inline void BindCpuForCurrentThread(const std::vector<uint32_t>& cpu_set) {
  if (cpu_set.empty()) {
    return;
  }

  static const uint32_t kMaxCpuIdx = std::thread::hardware_concurrency();
  for (auto cpu_idx : cpu_set) {
    AIMRT_ASSERT(cpu_idx < kMaxCpuIdx, "Invalid cpu index '{}', max cpu idx is '{}'", cpu_idx,
                 kMaxCpuIdx);
  }

  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  for (auto cpu_idx : cpu_set) {
    CPU_SET(cpu_idx, &cpuset);
  }

  auto ret = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
  AIMRT_ASSERT(ret == 0, "Call 'pthread_setaffinity_np' get error, ret code '{}'", ret);
}

inline void SetCpuSchedForCurrentThread(std::string_view sched) {
  if (sched.empty()) {
    return;
  }

  if (sched == "SCHED_OTHER") {
    // sched format: SCHED_OTHER
    struct sched_param param {
      .sched_priority = 0
    };
    int ret = pthread_setschedparam(pthread_self(), SCHED_OTHER, &param);
    AIMRT_ASSERT(ret == 0, "Call 'pthread_setschedparam' get error, ret code '{}'", ret);
  } else {
    // sched format: SCHED_FIFO:10 or SCHED_RR:10
    auto pos = sched.find_first_of(':');
    AIMRT_ASSERT(pos != std::string_view::npos && pos != (sched.size() - 1),
                 "Invalid sched parm '{}'", sched);

    auto sched_policy_str = sched.substr(0, pos);
    int policy = 0;
    if (sched_policy_str == "SCHED_FIFO") {
      policy = SCHED_FIFO;
    } else if (sched_policy_str == "SCHED_RR") {
      policy = SCHED_RR;
    } else {
      AIMRT_ASSERT(false, "Invalid sched parm '{}'", sched);
    }

    int priority_max = sched_get_priority_max(policy);
    int priority_min = sched_get_priority_min(policy);

    int sched_priority = atoi(sched.substr(pos + 1).data());
    AIMRT_ASSERT(sched_priority >= priority_min && sched_priority <= priority_max,
                 "Invalid sched priority '{}', required range {}~{}", sched_priority, priority_min,
                 priority_max);

    struct sched_param param {
      .sched_priority = sched_priority
    };

    int ret = pthread_setschedparam(pthread_self(), policy, &param);
    AIMRT_ASSERT(ret == 0, "Call 'pthread_setschedparam' get error, ret code '{}'", ret);
  }
}

}  // namespace nxpilot::utils::common
