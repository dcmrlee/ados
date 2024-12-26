// Copyright (C) 2024. All rights reserved.

#pragma once

#include <chrono>
#include <string>
#include <string_view>

namespace nxpilot::utils::common {

/**
 * @brief Nanoseconds
 *
 * @param t
 * @return uint64_t timestamp
 */
inline uint64_t GetTimestampNs(std::chrono::system_clock::time_point t) {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(t.time_since_epoch()).count();
}

/**
 * @brief Nanoseconds
 *
 * @return uint64_t timestamp
 */
inline uint64_t GetCurTimestampNs() { return GetTimestampNs(std::chrono::system_clock::now()); }

/**
 * @brief TimePoint from Nanoseconds
 *
 * @param ns timestamp
 * @return const std::chrono::system_clock::time_point
 */
inline const std::chrono::system_clock::time_point GetTimePointFromTimestampNs(uint64_t ns) {
  return std::chrono::system_clock::time_point(
      std::chrono::duration_cast<std::chrono::system_clock::time_point::duration>(
          std::chrono::nanoseconds(ns)));
}

/**
 * @brief Microseconds
 *
 * @param t
 * @return uint64_t timestamp
 */
inline uint64_t GetTimestampUs(std::chrono::system_clock::time_point t) {
  return std::chrono::duration_cast<std::chrono::microseconds>(t.time_since_epoch()).count();
}

/**
 * @brief Microseconds
 *
 * @return uint64_t timestamp
 */
inline uint64_t GetCurTimestampUs() { return GetTimestampUs(std::chrono::system_clock::now()); }

/**
 * @brief TimePoint from Microseconds
 *
 * @param ns timestamp
 * @return const std::chrono::system_clock::time_point
 */
inline const std::chrono::system_clock::time_point GetTimePointFromTimestampUs(uint64_t us) {
  return std::chrono::system_clock::time_point(
      std::chrono::duration_cast<std::chrono::system_clock::time_point::duration>(
          std::chrono::microseconds(us)));
}

/**
 * @brief Milliseconds
 *
 * @param t
 * @return uint64_t timestamp
 */
inline uint64_t GetTimestampMs(std::chrono::system_clock::time_point t) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(t.time_since_epoch()).count();
}

/**
 * @brief Milliseconds
 *
 * @return uint64_t timestamp
 */
inline uint64_t GetCurTimestampMs() { return GetTimestampMs(std::chrono::system_clock::now()); }

/**
 * @brief TimePoint from Milliseconds
 *
 * @param ns timestamp
 * @return const std::chrono::system_clock::time_point
 */
inline const std::chrono::system_clock::time_point GetTimePointFromTimestampMs(uint64_t ms) {
  return std::chrono::system_clock::time_point(
      std::chrono::duration_cast<std::chrono::system_clock::time_point::duration>(
          std::chrono::milliseconds(ms)));
}

/**
 * @brief Seconds
 *
 * @param t
 * @return uint64_t timestamp
 */
inline uint64_t GetTimestampSec(std::chrono::system_clock::time_point t) {
  return std::chrono::duration_cast<std::chrono::seconds>(t.time_since_epoch()).count();
}

/**
 * @brief Seconds
 *
 * @return uint64_t timestamp
 */
inline uint64_t GetCurTimestampSec() { return GetTimestampSec(std::chrono::system_clock::now()); }

/**
 * @brief TimePoint from Seconds
 *
 * @param ns timestamp
 * @return const std::chrono::system_clock::time_point
 */
inline const std::chrono::system_clock::time_point GetTimePointFromTimestampSec(uint64_t sec) {
  return std::chrono::system_clock::time_point(
      std::chrono::duration_cast<std::chrono::system_clock::time_point::duration>(
          std::chrono::seconds(sec)));
}

}  // namespace nxpilot::utils::common
