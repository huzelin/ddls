/*!
 * \file feeder.h
 * \brief The feeder for i/o reading.
 */
#pragma once

#include "hpps/feeder/plan.h"
#include "hpps/feeder/batch.h"
#include "hpps/feeder/blocking_queue.h"
#include "hpps/feeder/thread_pool.h"

namespace hpps {

class Feeder {
 public:
  static Feeder* Get() { static Feeder feeder; return &feeder; }

  void Start();
  void Stop();

  // Schedule one data source's sample.
  BlockingQueueIterator<Batch*>* Schedule(Plan* plan, int max_queue_size = 1);

 protected:
  void Run(int tid);

  // Each entry mean one data source.
  struct Entry {
    Plan* plan;
    BlockingQueue<Batch*>* blocking_queue;
  };
  void AddTask(const Entry& entry);

  // The task in data source.
  struct Task {
    size_t curr;
    Plan::Block block;
    BlockingQueue<Batch*>* blocking_queue;
  };

  std::vector<Entry> entries_;
  std::vector<Queue<Task>*> task_queues_;
  std::mutex mutex_;
  mutable bool running_;
};

}  // namespace hpps
