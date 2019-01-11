/*!
 * \file feeder.h
 * \brief The feeder for i/o reading.
 */
#include "hpps/feeder/feeder.h"

namespace hpps {

void Feeder::Stop() {
  running_ = false;
  for (auto i = 0; i < entries_.size(); i++) {
    task_queues_[i]->Exit();
  }
  delete thread_pool_;
  
  for (auto& entry : entries_) {
    delete entry.plan;
    delete entry.blocking_queue;
  }
  for (auto& task_queue : task_queues_) {
    delete task_queue;
  }
}

BlockingQueueIterator<std::unique_ptr<Batch>>* Feeder::Schedule(Plan* plan, int max_queue_size) {
  Entry entry;
  entry.plan = plan;
  entry.blocking_queue = new BlockingQueue<std::unique_ptr<Batch>>(max_queue_size);
  entries_.push_back(entry);
  AddTask(entry);
  return new BlockingQueueIterator<std::unique_ptr<Batch>>(entry.blocking_queue);
}

void Feeder::AddTask(const Feeder::Entry& entry) {
  task_queues_.resize(task_queues_.size() + 1, new Queue<Feeder::Task>());
  for (auto i = 0; i < entry.plan->sample_record.size(); ++i) {
    Feeder::Task task;
    task.sample_record = entry.plan->sample_record[i];
    task.blocking_queue = entry.blocking_queue;
    task.cache_batch_tensor = new std::queue<std::vector<Tensor*>>(); 
    task_queues_[task_queues_.size() - 1]->Push(task);
  }
}

void Feeder::Start(int thread_num) {
  running_ = true;
  thread_pool_ = new ThreadPool(thread_num, [this](int tid) { this->Run(tid); });
}

void Feeder::Run(int tid) {
  while (running_) {
    for (auto i = 0; i < entries_.size(); i++) {
      auto& entry = entries_[i];
      if (entry.blocking_queue->Full()) {
        continue;
      }
      // produce one batch for the data source.
      ProduceBatch(task_queues_[i]);
    }
  }
  LOG_INFO("Thread-%d is exiting ... ", tid);
}

void Feeder::ProduceBatch(Queue<Task>* queue) {
  Task task;
  if (!queue->Pop(task)) return;
  
  if (task.curr + task.sample_record.plan->batch_size <=
      task.sample_record.record_io->sample_count()) {
    LOG_DEBUG("task.curr=%u sample_count=%u cache_batch_tensor=%p",
              task.curr,
              task.sample_record.record_io->sample_count(),
              task.cache_batch_tensor);

    // Step1: init batch
    std::unique_ptr<Batch> batch(new Batch(task.sample_record.record_io->names()));
    
    // Step2: merge block and generate batch
    auto size = AssembleBatch(task.sample_record, *(task.cache_batch_tensor), batch);
    task.curr += size;
    
    task.blocking_queue->Push(batch);
    if (task.curr < task.sample_record.record_io->sample_count()) {
      queue->PushFront(task);
    }
  }
}

int Feeder::AssembleBatch(Plan::SampleRecord& sample_record,
                          std::queue<std::vector<Tensor*>>& cache_batch_tensor,
                          std::unique_ptr<Batch>& batch) {
  // Step1: Read batch tensor
  std::vector<std::vector<Tensor*>> batch_tensor = ReadBatchTensor(sample_record, cache_batch_tensor);

  // Step2: Batch tensor2 batch
  BatchTensor2Batch(batch_tensor, sample_record, batch);

  // Step3: Release
  for (auto& tensors : batch_tensor) {
    for (auto& tensor : tensors) {
      delete tensor;
    }
  }
  batch_tensor.clear();

  auto batch_size = sample_record.plan->batch_size;
  return batch_size;
}

std::vector<std::vector<Tensor*>> Feeder::ReadBatchTensor(Plan::SampleRecord& sample_record,
                                                          std::queue<std::vector<Tensor*>>& cache_batch_tensor) {
  std::vector<std::vector<Tensor*>> batch_tensor;
  size_t remaining_size = sample_record.plan->batch_size;
  
  std::vector<Tensor*> item;
  size_t curr_batch_size = 0;

  while (remaining_size) {
    if (!cache_batch_tensor.empty()) {
      item = cache_batch_tensor.front();
      cache_batch_tensor.pop();
    } else {
      item = sample_record.record_io->ReadSampleAsArray();
    }

    curr_batch_size = 0;
    for (auto i = 0; i < item.size(); ++i) {
      if (sample_record.record_io->IsPrimary(i)) {
        curr_batch_size = item[i]->shape()[0];
        break;
      }
    }
    if (remaining_size < curr_batch_size) {
      break;
    } else {
      batch_tensor.push_back(item);
      remaining_size -= curr_batch_size;
    }
  }

  if (remaining_size) {
    auto splits = sample_record.record_io->Split(item, remaining_size);
    CHECK(splits.size() == 2);
    batch_tensor.push_back(splits[0]);
    cache_batch_tensor.push(splits[1]);
  }
  return batch_tensor;
}

void Feeder::BatchTensor2Batch(std::vector<std::vector<Tensor*>>& batch_tensor,
                               Plan::SampleRecord& sample_record,
                               std::unique_ptr<Batch>& batch) {
  for (auto col = 0; col < batch_tensor[0].size(); ++col) {
    auto row = 0;
    std::vector<tensor_dim_t> shape = batch_tensor[row++][col]->shape();
    std::vector<size_t> offsets;
    offsets.push_back(0);
    size_t stride = batch_tensor[0][col]->size();

    for (; row < batch_tensor.size(); ++row) {
      const std::vector<tensor_dim_t>& shape_tmp = batch_tensor[row][col]->shape();
      CHECK(shape_tmp.size() == shape.size());
      for (auto i = 1; i < shape_tmp.size(); ++i) {
        CHECK(shape[i] == shape_tmp[i]);
      }
      shape[0] += shape_tmp[0];
      offsets.push_back(stride);
      stride += batch_tensor[row][col]->size();
    }
    Tensor* result = new Tensor(shape, sample_record.record_io->configs()[col].type);
    memset(result->mutable_blob()->data(), 0, result->mutable_blob()->size());

    auto type_size = TensorDataTypeSize(sample_record.record_io->configs()[col].type);
    for (row = 0; row < batch_tensor.size(); ++row) {
      Tensor* src = batch_tensor[row][col];
      Tensor* dst = result;
      memcpy(dst->mutable_blob()->data() + offsets[row] * type_size,
             src->mutable_blob()->data(),
             src->size() * type_size);  // The src tensor will be reshaped.
    }
    batch->Set(col, result);
  }
}

}  // namespace hpps
