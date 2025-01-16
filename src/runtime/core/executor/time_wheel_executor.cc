// Copyright (C) 2024. All rights reserved.

#include "runtime/core/executor/time_wheel_executor.h"
#include "utils/common/thread_tool.h"

namespace YAML {
template <>
struct convert<nxpilot::runtime::core::executor::TimeWheelExecutor::Options> {
  using Options = nxpilot::runtime::core::executor::TimeWheelExecutor::Options;

  static Node encode(const Options& rhs) {
    Node node;

    node["bind_executor"] = rhs.bind_executor;
    node["thread_sched_policy"] = rhs.thread_sched_policy;
    node["thread_bind_cpu"] = rhs.thread_bind_cpu;
    node["dt_us"] = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(rhs.dt).count());
    node["wheel_size"] = rhs.wheel_size;

    return node;
  }

  static bool decode(const Node& node, Options& rhs) {
    if (!node.IsMap()) return false;

    if (node["bind_executor"]) rhs.bind_executor = node["bind_executor"].as<std::string>();
    if (node["thread_sched_policy"])
      rhs.thread_sched_policy = node["thread_sched_policy"].as<std::string>();
    if (node["thread_bind_cpu"])
      rhs.thread_bind_cpu = node["thread_bind_cpu"].as<std::vector<uint32_t>>();
    if (node["dt_us"]) rhs.dt = std::chrono::microseconds(node["dt_us"].as<uint64_t>());
    if (node["wheel_size"]) rhs.wheel_size = node["wheel_size"].as<std::vector<size_t>>();

    return true;
  }
};
}  // namespace YAML

namespace nxpilot::runtime::core::executor {

void TimeWheelExecutor::Initialize(std::string_view name, YAML::Node options_node) {
  NXPILOT_CHECK_ERROR(std::atomic_exchange(&state_, State::kInit) == State::kPreInit,
                      "TimeWheelExecutor can only be initialized once.");
  name_ = std::string(name);
  if (options_node && !options_node.IsNull()) {
    options_ = options_node.as<Options>();
  }

  dt_count_ = static_cast<uint64_t>(
      std::chrono::duration_cast<std::chrono::nanoseconds>(options_.dt).count());

  uint64_t cur_scale = 1;
  for (size_t ii = 0; ii < options_.wheel_size.size(); ++ii) {
    uint64_t start_pos = (ii == 0) ? 0 : 1;
    timing_wheel_vec_.emplace_back(TimingWheelTool{
        .current_pos = start_pos,
        .scale = (cur_scale *= options_.wheel_size[ii]),
        .wheel = std::vector<TaskList>(options_.wheel_size[ii]),
        .borrow_func = [ii, this]() {
          TaskList task_list;
          if (ii < options_.wheel_size.size() - 1) {
            task_list = timing_wheel_vec_[ii + 1].Tick();
          } else {
            auto iter = timing_task_map_.find(timing_task_map_pos_);
            ++timing_task_map_pos_;
            if (iter != timing_task_map_.end()) {
              task_list = std::move(iter->second);
              timing_task_map_.erase(iter);
            }

            while (!task_list.empty()) {
              auto iter = task_list.begin();
              auto& cur_list =
                  timing_wheel_vec_[ii].wheel[iter->tick_count % timing_wheel_vec_[ii].scale];
              cur_list.splice(cur_list.end(), task_list, iter);
            }
          }
        }});
  }

  timing_task_map_pos_ = 1;

  NXPILOT_INFO("TimeWheelExecutor init completed");
}

void TimeWheelExecutor::Start() {
  NXPILOT_CHECK_ERROR(std::atomic_exchange(&state_, State::kStart) == State::kInit,
                      "TimeWheelExecutor can only run when state is 'Init'.");

  timer_thread_ptr_ = std::make_unique<std::thread>(std::bind(&TimeWheelExecutor::TimerLoop, this));

  start_flag_.wait(false);
  NXPILOT_INFO("TimeWheelExecutor start completed");
}

void TimeWheelExecutor::Shutdown() {
  if (std::atomic_exchange(&state_, State::kShutdown) == State::kShutdown) {
    return;
  }

  if (timer_thread_ptr_ && timer_thread_ptr_->joinable()) {
    timer_thread_ptr_->join();
  }

  timer_thread_ptr_.reset();
  timing_task_map_.clear();
  timing_wheel_vec_.clear();

  NXPILOT_INFO("TimeWheelExecutor shutdown");
}

void TimeWheelExecutor::Execute(Task&& task) noexcept {
  NXPILOT_ERROR("TimeWheelExecutor does not impl execute");
}

std::chrono::system_clock::time_point TimeWheelExecutor::Now() const noexcept {
  std::shared_lock<std::shared_mutex> lck(tick_mutex_);

  return nxpilot::utils::common::GetTimePointFromTimestampNs(current_tick_count_ * dt_count_ +
                                                             start_time_point_);
}

void TimeWheelExecutor::ExecuteAt(std::chrono::system_clock::time_point tp, Task&& task) noexcept {
  try {
    uint64_t virtual_tp = nxpilot::utils::common::GetTimestampNs(tp) - start_time_point_;

    std::unique_lock<std::shared_mutex> lck(tick_mutex_);

    if (virtual_tp < current_tick_count_ * dt_count_) {
      lck.unlock();
      Execute(std::move(task));
      return;
    }

    uint64_t tmp_current_tick_count = current_tick_count_;
    uint64_t diff_tick_count = virtual_tp / dt_count_ - current_tick_count_;

    const size_t len = options_.wheel_size.size();
    for (size_t ii = 0; ii < len; ++ii) {
      if (diff_tick_count < options_.wheel_size[ii]) {
        auto pos = (diff_tick_count + tmp_current_tick_count) % options_.wheel_size[ii];
        timing_wheel_vec_[ii].wheel[pos].emplace_back(
            TaskWithTimestamp{virtual_tp / dt_count_, std::move(task)});
        return;
      }

      diff_tick_count /= options_.wheel_size[ii];
      tmp_current_tick_count /= options_.wheel_size[ii];
    }
    timing_task_map_[diff_tick_count + tmp_current_tick_count].emplace_back(
        TaskWithTimestamp{virtual_tp / dt_count_, std::move(task)});
  } catch (const std::exception& e) {
    NXPILOT_ERROR("{}", e.what());
  }
}

void TimeWheelExecutor::TimerLoop() {
  thread_id_ = std::this_thread::get_id();

  try {
    nxpilot::utils::common::SetNameForCurrentThread(name_);
    nxpilot::utils::common::BindCpuForCurrentThread(options_.thread_bind_cpu);
    nxpilot::utils::common::SetCpuSchedForCurrentThread(options_.thread_sched_policy);
  } catch (const std::exception& e) {
    NXPILOT_ERROR("Set thread policy for TimeWheelExecutor get exception, {}", e.what());
  }

  auto last_loop_time_point = std::chrono::system_clock::now();

  start_time_point_ = nxpilot::utils::common::GetTimestampNs(last_loop_time_point);
  start_flag_.store(true);
  start_flag_.notify_all();

  while (state_.load() != State::kShutdown) {
    try {
      auto real_dt = options_.dt;
      do {
        static constexpr auto kMaxSleepDt = std::chrono::seconds(1);
        auto sleep_time = (real_dt > kMaxSleepDt) ? kMaxSleepDt : real_dt;
        real_dt -= sleep_time;

        if (real_dt.count() && options_.dt < kMaxSleepDt && real_dt <= options_.dt) {
          sleep_time += real_dt;
          real_dt -= real_dt;
        }

        std::this_thread::sleep_until(
            last_loop_time_point +=
            std::chrono::duration_cast<std::chrono::system_clock::time_point::duration>(
                sleep_time));

      } while (state_.load() != State::kShutdown && real_dt.count());

      tick_mutex_.lock();
      TaskList task_list = timing_wheel_vec_[0].Tick();
      if (!task_list.empty()) {
        tick_mutex_.unlock();
        for (auto& iter : task_list) {
          try {
            iter.task();
          } catch (const std::exception& e) {
            NXPILOT_FATAL("TimeWheelExecutor run task get exception, {}", e.what());
          }
        }
        tick_mutex_.lock();
      }

      ++current_tick_count_;
      tick_mutex_.unlock();

    } catch (const std::exception& e) {
      NXPILOT_FATAL("TimeWheelExecutor run loop get exception, {}", e.what());
    }
  }
}

}  // namespace nxpilot::runtime::core::executor