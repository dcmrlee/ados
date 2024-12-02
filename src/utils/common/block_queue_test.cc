// Copyright (C) 2024. All rights reserved.

#include <gtest/gtest.h>

#include "utils/common/block_queue.h"

namespace nxpilot::utils::common {

TEST(BlockQueueTest, EnqueueDequeue) {
  BlockQueue<int> queue;
  queue.Enqueue(1);
  ASSERT_EQ(1, queue.Dequeue());
}

}  // namespace nxpilot::utils::common
