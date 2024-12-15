// Copyright (C) 2024. All rights reserved.

#include "gtest/gtest.h"

#include "utils/common/time_util.h"

namespace nxpilot::utils::common {

TEST(TIME_UTIL_TEST, Timestamp_test) {
  auto tp = std::chrono::system_clock::now();
  uint64_t ns_timestamp = GetTimestampNs(tp);
  auto ns_tp = GetTimePointFromTimestampNs(ns_timestamp);
  EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(ns_tp.time_since_epoch()).count(),
            ns_timestamp);

  uint64_t us_timestamp = GetTimestampUs(tp);
  auto us_tp = GetTimePointFromTimestampUs(us_timestamp);
  EXPECT_EQ(std::chrono::duration_cast<std::chrono::microseconds>(us_tp.time_since_epoch()).count(),
            us_timestamp);

  uint64_t ms_timestamp = GetTimestampMs(tp);
  auto ms_tp = GetTimePointFromTimestampMs(ms_timestamp);
  EXPECT_EQ(std::chrono::duration_cast<std::chrono::milliseconds>(ms_tp.time_since_epoch()).count(),
            ms_timestamp);

  uint64_t sec_timestamp = GetTimestampSec(tp);
  auto sec_tp = GetTimePointFromTimestampSec(sec_timestamp);
  EXPECT_EQ(std::chrono::duration_cast<std::chrono::seconds>(sec_tp.time_since_epoch()).count(),
            sec_timestamp);
}

}  // namespace nxpilot::utils::common
