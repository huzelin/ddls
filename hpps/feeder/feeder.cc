/*!
 * \file feeder.h
 * \brief The feeder for i/o reading.
 */
#include "hpps/feeder/feeder.h"

namespace hpps {

Feeder::~Feeder() {
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

BlockingQueueIterator<std::shared_ptr<Batch>>* Feeder::Schedule(Plan* plan, int max_queue_size) {
  Entry entry;
  entry.plan = plan;
  entry.blocking_queue = new BlockingQueue<std::shared_ptr<Batch>>(max_queue_size);
  entries_.push_back(entry);
  AddTask(entry);
  return new BlockingQueueIterator<std::shared_ptr<Batch>>(entry.blocking_queue);
}

void Feeder::AddTask(const Feeder::Entry& entry) {
  task_queues_.resize(task_queues_.size() + 1, new Queue<Feeder::Task>());
  for (auto i = 0; i < entry.plan->sample_record.size(); ++i) {
    Feeder::Task task;
    task.sample_record = entry.plan->sample_record[i];
    task.blocking_queue = entry.blocking_queue;
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
      if (entry.blocking_queue->Full()) continue;
      // produce one batch for the data source.
      ProduceBatch(task_queues_[i]);
    }
  }
}

void Feeder::ProduceBatch(Queue<Task>* queue) {
  Task task;
  if (!queue->Pop(task)) return;
  
  if (task.curr + task.sample_record.plan->batch_size <=
      task.sample_record.record_io->sample_count()) {
    // Step1: init batch
    std::shared_ptr<Batch> batch(new Batch(task.sample_record.record_io->names()));
    
    // Step2: merge block and generate batch
    auto size = AssembleBatch(task.sample_record, batch);
    task.curr += size;
    
    task.blocking_queue->Push(batch);
    if (task.curr < task.sample_record.record_io->sample_count()) {
      queue->PushFront(task);
    }
  }
}

int Feeder::AssembleBatch(Plan::SampleRecord& sample_record, std::shared_ptr<Batch>& batch) {
  // Step1: Read batch tensor
  std::vector<std::vector<Tensor*>> batch_tensor = ReadBatchTensor(sample_record);

  // Step2: Batch tensor2 batch
  BatchTensor2Batch(batch_tensor, sample_record, batch);

  // Step3: Release
  for (auto& tensors : batch_tensor) {
    for (auto& tensor : tensors) {
      delete tensor;
    }
  }
  return sample_record.plan->batch_size;
}

std::vector<std::vector<Tensor*>> Feeder::ReadBatchTensor(Plan::SampleRecord& sample_record) {
  std::vector<std::vector<Tensor*>> batch_tensor;
  for (int i = 0; i < sample_record.plan->batch_size; ++i) {
    batch_tensor.push_back(sample_record.record_io->ReadSampleAsArray());
  }
  return batch_tensor;
}

void Feeder::BatchTensor2Batch(std::vector<std::vector<Tensor*>>& batch_tensor,
                               Plan::SampleRecord& sample_record,
                               std::shared_ptr<Batch>& batch) {
  for (auto col = 0; col < batch_tensor[0].size(); ++col) {
    auto row = 0;
    std::vector<tensor_dim_t> shape = batch_tensor[row++][col]->shape();
    for (; row < batch_tensor.size(); ++row) {
      const std::vector<tensor_dim_t>& shape_tmp = batch_tensor[row][col]->shape();
      CHECK(shape_tmp.size() == shape.size());
      for (auto i = 0; i < shape_tmp.size(); ++i) {
        CHECK(shape[i] == shape_tmp[i]);
      }
    }
    size_t stride = 1;
    for (auto dim : shape) stride *= dim;
    stride *= TensorDataTypeSize(sample_record.record_io->types()[col]);

    shape.insert(shape.begin(), batch_tensor.size());
    Tensor* result = new Tensor(shape, sample_record.record_io->types()[col]);
    memset(result->mutable_blob()->data(), 0, result->mutable_blob()->size());

    for (row = 0; row < batch_tensor.size(); ++row) {
      Tensor* src = batch_tensor[row][col];
      Tensor* dst = result;
      memcpy(dst->mutable_blob()->data() + row * stride,
             src->mutable_blob()->data(),
             src->mutable_blob()->size());
    }
    batch->Set(col, result);
  }
}

}  // namespace hpps
