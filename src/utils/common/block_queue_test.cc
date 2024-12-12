// Copyright (C) 2024. All rights reserved.

#include <gtest/gtest.h>

#include "utils/common/block_queue.h"

namespace nxpilot::utils::common {

TEST(BlockQueueTest, EnqueueDequeue) {
  BlockQueue<int> queue;
  queue.Enqueue(1);
  ASSERT_EQ(1, queue.Dequeue());
}

TEST(BlockQueueTest, TryDequeueEmpty) {
  BlockQueue<int> queue;
  auto result = queue.TryDequeue();
  ASSERT_EQ(std::nullopt, result);
}

TEST(BlockQueueTest, TryDequeueNonEmpty) {
  BlockQueue<int> queue;
  queue.Enqueue(1);
  auto result = queue.TryDequeue();
  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(1, result.value());
}

TEST(BlockQueueTest, Stop) {
  BlockQueue<int> queue;

  queue.Stop();

  ASSERT_THROW(queue.Enqueue(1), BlockQueueStoppedException);
  ASSERT_THROW(queue.Dequeue(), BlockQueueStoppedException);
}

TEST(BlockQueueTest, MultipleThreadsEnqueue) {
  BlockQueue<int> queue;
  std::vector<std::thread> threads;

  threads.emplace_back([&]() {
    for (int i = 0; i < 1000; ++i) {
      queue.Enqueue(i);
    }
  });

  threads.emplace_back([&]() {
    for (int i = 0; i < 1000; ++i) {
      queue.Enqueue(i);
    }
  });

  for (auto& t : threads) {
    t.join();
  }

  ASSERT_EQ(2000, queue.Size());
}

TEST(BlockQueueTest, MultipleThreadsEnqueueDequeue) {
  BlockQueue<int> queue;
  std::vector<std::thread> threads;
  std::vector<int> results;

  threads.emplace_back([&]() {
    for (int i = 0; i < 1000; ++i) {
      queue.Enqueue(i);
    }
  });

  threads.emplace_back([&]() {
    for (int i = 0; i < 1000; ++i) {
      auto item = queue.Dequeue();
      results.push_back(item);
    }
  });

  for (auto& t : threads) {
    t.join();
  }

  for (int i = 0; i < 1000; ++i) {
    ASSERT_EQ(i, results[i]);
  }

}

}  // namespace nxpilot::utils::common
