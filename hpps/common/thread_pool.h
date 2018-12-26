/*!
 * \file thread_pool.h
 * \brief The thread pool for feeder
 */
#pragma once

#include <thread>
#include <vector>

namespace hpps {

class ThreadPool {
 public:
  explicit ThreadPool(size_t size, std::function<void(int)> func)
      : worker_threads_(size) {
    int idx = 0;
    for (auto& i : worker_threads_) {
      i = std::thread(func, idx++);
    }
  }
  virtual ~ThreadPool() noexcept(false) {
    for (auto&& i : worker_threads_) {
      i.join();
    }
  }
  size_t Size() const {
    return worker_threads_.size();
  }

 private:
  std::vector<std::thread> worker_threads_;
  ThreadPool() = delete;
};

}  // namespace hpps
