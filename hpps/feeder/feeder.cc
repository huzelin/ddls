/*!
 * \file feeder.h
 * \brief The feeder for i/o reading.
 */
#include "hpps/feeder/feeder.h"

namespace hpps {

BlockingQueueIterator<Batch*>* Feeder::Schedule(Plan* plan, int max_queue_size) {
  Entry entry;
  entry.plan = plan;
  entry.blocking_queue = new BlockingQueue<Batch*>(max_queue_size);
  entries_.push_back(entry);
  AddTask(entry);
  return new BlockingQueueIterator<Batch*>(entry.blocking_queue);
}

void Feeder::AddTask(const Feeder::Entry& entry) {
  task_queues_.resize(task_queues_.size() + 1, new Queue<Feeder::Task>());
  for (auto i = 0; i < entry.plan->block.size(); ++i) {
    Feeder::Task task;
    task.block = entry.plan->block[i];
    task.blocking_queue = entry.blocking_queue;
    task_queues_[task_queues_.size() - 1]->Push(task);
  }
}

void Feeder::Start() {
  running_ = true;
}

void Feeder::Stop() {
  running_ = false;
}

void Feeder::Run(int tid) {
  while (running_) {
    for (auto i = 0; i < entries_.size(); i++) {
      auto& entry = entries_[i];
      if (entry.blocking_queue->Full()) continue;
      // Must generate one batch
    }
  }
}

}  // namespace hpps
