/*!
 * \file blocking_queue.h
 * \brief The batch queue
 */
#pragma once

#include "hpps/common/queue.h"

namespace hpps {

template <typename T>
class BlockingQueue {
 public:
  explicit BlockingQueue(uint32_t max_queue_size = 1) : max_queue_size_(max_queue_size) { }

  // Return the mutable queue
  Queue<T>* mutable_queue() { return &queue_; }

  // Push item into queue
  void Push(T& item) { queue_.Push(item); }

  // Return true if the Size >= max_queue_size_
  bool Full() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.Size() >= max_queue_size_) return true;
    return false;
  }

 protected:
  Queue<T> queue_;
  uint32_t max_queue_size_;
  mutable std::mutex mutex_;
  std::condition_variable full_condition_;
};

template <typename T>
class BlockingQueueIterator {
 public:
  BlockingQueueIterator(BlockingQueue<T>* queue) : queue_(queue) { }

  bool Pop(T& result) {
    return queue_->mutable_queue()->Pop(result);
  }

 protected:
  BlockingQueue<T>* queue_;
};

}  // namespace hpps
