// Copyright (C) 2024. All rights reserved.

#pragma once

#include <sys/syscall.h>
#include <unistd.h>

#include <chrono>
#include <functional>
#include <source_location>
#include <string>

#include "utils/common/exception.h"
#include "utils/common/time_util.h"

namespace nxpilot::utils::common {

constexpr uint32_t kLogLevelTrace = 0;
constexpr uint32_t kLogLevelDebug = 1;
constexpr uint32_t kLogLevelInfo = 2;
constexpr uint32_t kLogLevelWarn = 3;
constexpr uint32_t kLogLevelError = 4;
constexpr uint32_t kLogLevelFatal = 5;

class InternalLoggerImpl {
 public:
  static uint32_t GetLogLevel() { return 0; }
  static void Log(uint32_t lvl, uint32_t line, uint32_t column, const char* file_name,
                  const char* function_name, const char* log_data, size_t log_data_size) {
    static constexpr std::string_view kLvlNameArray[] = {"Trace", "Debug", "Info",
                                                         "Warn",  "Error", "Fatal"};
    static constexpr uint32_t kLvlNameArraySize = sizeof(kLvlNameArray) / sizeof(kLvlNameArray[0]);
    lvl = lvl >= kLvlNameArraySize ? (kLvlNameArraySize - 1) : lvl;

    // Only for Linux
    thread_local size_t tid(syscall(SYS_gettid));
    thread_local auto now = std::chrono::system_clock::now();
    static const std::chrono::time_zone* current_zone = std::chrono::current_zone();
    AIMRT_ASSERT(current_zone != nullptr, "Cannot get time zone");

    thread_local std::chrono::zoned_time zt{current_zone, now};
    std::string log_str = std::format("[{:%Y-%m-%d %H:%M:%S}][{}][{}][{}:{}:{}]{} ", zt,
                                      kLvlNameArray[lvl], tid, file_name, line, column,
                                      std::string_view(log_data, log_data_size));
    fprintf(stderr, "%s\n", log_str.c_str());
  }
};

struct Logger {
  uint32_t GetLogLevel() const { return get_log_level_func(); }

  void Log(uint32_t lvl, uint32_t line, uint32_t column, const char* file_name,
           const char* function_name, const char* log_data, size_t log_data_size) const {
    log_func(lvl, line, column, file_name, function_name, log_data, log_data_size);
  }

  using GetLogLevelFunc = std::function<uint32_t(void)>;
  using LogFunc = std::function<void(uint32_t, uint32_t, uint32_t, const char*, const char*,
                                     const char*, size_t)>;

  GetLogLevelFunc get_log_level_func = InternalLoggerImpl::GetLogLevel;
  LogFunc log_func = InternalLoggerImpl::Log;
};

}  // namespace nxpilot::utils::common

#define NXPILOT_HANDLE_LOG(__lgr__, __lvl__, __fmt__, ...)                                         \
  do {                                                                                             \
    const auto& __cur_lgr__ = __lgr__;                                                             \
    if (__lvl__ >= __cur_lgr__.GetLogLevel()) {                                                    \
      std::string __log_str__ = std::format(__fmt__, ##__VA_ARGS__);                               \
      constexpr auto __location__ = std::source_location::current();                               \
      __cur_lgr__.Log(__lvl__, __location__.line(), __location__.column(),                         \
                      __location__.file_name(), __location__.function_name(), __log_str__.c_str(), \
                      __log_str__.size());                                                         \
    }                                                                                              \
  } while (0)

#define NXPILOT_HANDLE_CHECK_LOG(__lgr__, __expr__, __lvl__, __fmt__, ...)      \
  do {                                                                          \
    if (!(__expr__)) [[unlikely]] {                                             \
      std::string __log_str__ = std::format(__fmt__, ##__VA_ARGS__);            \
      const auto& __cur_lgr__ = __lgr__;                                        \
      if (__lvl__ >= __cur_lgr__.GetLogLevel()) {                               \
        constexpr auto __location__ = std::source_location::current();          \
        __cur_lgr__.Log(__lvl__, __location__.line(), __location__.column(),    \
                        __location__.file_name(), __location__.function_name(), \
                        __log_str__.c_str(), __log_str__.size());               \
      }                                                                         \
      throw nxpilot::utils::common::NxpilotException(std::move(__log_str__));   \
    }                                                                           \
  } while (0)

#define NXPILOT_DEFAULT_LOGGER_HANDLE GetLogger()

#define NXPILOT_TRACE(__fmt__, ...)                                                         \
  NXPILOT_HANDLE_LOG(NXPILOT_DEFAULT_LOGGER_HANDLE, nxpilot::utils::common::kLogLevelTrace, \
                     __fmt__, ##__VA_ARGS__)
#define NXPILOT_DEBUG(__fmt__, ...)                                                         \
  NXPILOT_HANDLE_LOG(NXPILOT_DEFAULT_LOGGER_HANDLE, nxpilot::utils::common::kLogLevelDebug, \
                     __fmt__, ##__VA_ARGS__)
#define NXPILOT_INFO(__fmt__, ...)                                                         \
  NXPILOT_HANDLE_LOG(NXPILOT_DEFAULT_LOGGER_HANDLE, nxpilot::utils::common::kLogLevelInfo, \
                     __fmt__, ##__VA_ARGS__)
#define NXPILOT_WARN(__fmt__, ...)                                                         \
  NXPILOT_HANDLE_LOG(NXPILOT_DEFAULT_LOGGER_HANDLE, nxpilot::utils::common::kLogLevelWarn, \
                     __fmt__, ##__VA_ARGS__)
#define NXPILOT_ERROR(__fmt__, ...)                                                         \
  NXPILOT_HANDLE_LOG(NXPILOT_DEFAULT_LOGGER_HANDLE, nxpilot::utils::common::kLogLevelError, \
                     __fmt__, ##__VA_ARGS__)
#define NXPILOT_FATAL(__fmt__, ...)                                                         \
  NXPILOT_HANDLE_LOG(NXPILOT_DEFAULT_LOGGER_HANDLE, nxpilot::utils::common::kLogLevelFatal, \
                     __fmt__, ##__VA_ARGS__)

#define NXPILOT_CHECK_TRACE(__expr__, __fmt__, ...)                 \
  NXPILOT_HANDLE_CHECK_LOG(NXPILOT_DEFAULT_LOGGER_HANDLE, __expr__, \
                           nxpilot::utils::common::kLogLevelTrace, __fmt__, ##__VA_ARGS__)
#define NXPILOT_CHECK_DEBUG(__expr__, __fmt__, ...)                 \
  NXPILOT_HANDLE_CHECK_LOG(NXPILOT_DEFAULT_LOGGER_HANDLE, __expr__, \
                           nxpilot::utils::common::kLogLevelDebug, __fmt__, ##__VA_ARGS__)
#define NXPILOT_CHECK_INFO(__expr__, __fmt__, ...)                  \
  NXPILOT_HANDLE_CHECK_LOG(NXPILOT_DEFAULT_LOGGER_HANDLE, __expr__, \
                           nxpilot::utils::common::kLogLevelInfo, __fmt__, ##__VA_ARGS__)
#define NXPILOT_CHECK_WARN(__expr__, __fmt__, ...)                  \
  NXPILOT_HANDLE_CHECK_LOG(NXPILOT_DEFAULT_LOGGER_HANDLE, __expr__, \
                           nxpilot::utils::common::kLogLevelWarn, __fmt__, ##__VA_ARGS__)
#define NXPILOT_CHECK_ERROR(__expr__, __fmt__, ...)                 \
  NXPILOT_HANDLE_CHECK_LOG(NXPILOT_DEFAULT_LOGGER_HANDLE, __expr__, \
                           nxpilot::utils::common::kLogLevelError, __fmt__, ##__VA_ARGS__)
#define NXPILOT_CHECK_FATAL(__expr__, __fmt__, ...)                 \
  NXPILOT_HANDLE_CHECK_LOG(NXPILOT_DEFAULT_LOGGER_HANDLE, __expr__, \
                           nxpilot::utils::common::kLogLevelFatal, __fmt__, ##__VA_ARGS__)
