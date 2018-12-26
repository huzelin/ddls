/*!
 * \file feeder.h
 * \brief The feeder for i/o reading.
 */
#include "hpps/feeder/feeder.h"

namespace hpps {

Feeder::~Feeder() {
  running_ = false;
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
  for (auto i = 0; i < entry.plan->block.size(); ++i) {
    Feeder::Task task;
    task.block = entry.plan->block[i];
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
  queue->Pop(task);
  if (task.curr + task.block.plan->batch_size > task.block.count) {
    // batch can not be assembled, so throw the samples.
    delete task.block.stream;
  } else {
    // Step1: init batch
    auto plan = task.block.plan;
    std::shared_ptr<Batch> batch(new Batch(plan->tensor_names));
    
    // Step2: merge block and generate batch
    for (auto i = 0; i < task.block.plan->batch_size; ) {
      auto next_block_size = AssembleBatch(task.block.stream, task.block.plan, batch);
      task.curr += next_block_size;
      i += next_block_size;
    }
    task.blocking_queue->Push(batch);
    if (task.curr == task.block.count) {
      delete task.block.stream;
    } else {
      queue->PushFront(task);
    }
  }
}

int Feeder::AssembleBatch(Stream* stream, Plan* plan, std::shared_ptr<Batch>& batch) {
  // Step1: Read batch tensor
  std::vector<std::vector<Tensor*>> batch_tensor = ReadBatchTensor(stream, plan);

  // Step2: Assemble Batch
  AssembleBatch(batch_tensor, plan, batch);

  // Step3: Release
  for (auto& tensors : batch_tensor) {
    for (auto& tensor : tensors) {
      delete tensor;
    }
  }
  return plan->batch_size;
}

std::vector<std::vector<Tensor*>> Feeder::ReadBatchTensor(Stream* stream, Plan* plan) {
  std::vector<std::vector<Tensor*>> batch_tensor;
  
  for (int i = 0; i < plan->batch_size; ++i) {
    tensor_count_t tensor_count;
    CHECK(sizeof(tensor_count) == stream->Read(&tensor_count, sizeof(tensor_count)));
    CHECK(tensor_count == plan->tensor_data_types.size());

    std::vector<Tensor*> tensors;
    for (auto k = 0; k < tensor_count; ++k) {
      tensor_dim_t dim_size;
      CHECK(sizeof(dim_size) == stream->Read(&dim_size, sizeof(dim_size)));

      std::vector<tensor_dim_t> shape;
      for (auto m = 0; m < dim_size; ++m) {
        tensor_dim_t dim;
        CHECK(sizeof(dim) == stream->Read(&dim, sizeof(dim)));
        shape.push_back(dim);
      }
      
      auto tensor_data_type = plan->tensor_data_types[k];
      auto tensor = new Tensor(shape, tensor_data_type);
      tensors.push_back(tensor);

      auto blob = tensor->mutable_blob();
      char* addr = blob->data();
      CHECK(blob->size() == stream->Read(addr, blob->size()));        
    }

    if (batch_tensor.size() > 0) {
      CHECK(tensors.size() == batch_tensor[0].size());
    }
    batch_tensor.push_back(tensors);
  }
  return batch_tensor;
}

void Feeder::AssembleBatch(std::vector<std::vector<Tensor*>>& batch_tensor,
                           Plan* plan,
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
    stride *= TensorDataTypeSize(plan->tensor_data_types[col]);

    shape.insert(shape.begin(), batch_tensor.size());
    Tensor* result = new Tensor(shape, plan->tensor_data_types[col]);
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
