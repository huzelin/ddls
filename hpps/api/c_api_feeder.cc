/*
 * \file c_api_feeder.cc
 */

#include "hpps/api/c_api_feeder.h"

#include "hpps/api/c_api_error.h"
#include "hpps/feeder/feeder.h"
#include "hpps/feeder/record_io.h"

using namespace hpps;

// Record I/O
int HPPS_RecordIOCreate(const char* uri, int mode, Handle* record_io) {
  FileOpenMode file_open_mode = static_cast<FileOpenMode>(mode);
  *record_io = new RecordIO(uri, file_open_mode);
  if (*record_io == nullptr) {
    HPPS_SetLastErrorString("Create RecordIO Failed");
    return -1;
  }
  return 0;
}

int HPPS_WriteHeader(Handle handle, int num, const char** name, const int* type) {
  RecordIO* record_io = reinterpret_cast<RecordIO*>(handle);
  std::unordered_map<std::string, tensor_data_type_t> tensor_meta;
  for (auto i = 0; i < num; ++i) {
    tensor_meta[name[i]] = type[i];
  }
  record_io->WriteHeader(tensor_meta);
  return 0;
}

int HPPS_WriteSample(Handle handle, int num, const char** name, Handle* tensor) {
  RecordIO* record_io = reinterpret_cast<RecordIO*>(handle);
  std::unordered_map<std::string, Tensor*> sample;
  for (auto i = 0; i < num; ++i) {
    sample[name[i]] = reinterpret_cast<Tensor*>(tensor[i]);
  }
  record_io->WriteSample(sample);
  return 0;
}

int HPPS_WriteFinalize(Handle handle) {
  RecordIO* record_io = reinterpret_cast<RecordIO*>(handle);
  record_io->WriteFinalize();
  return 0;
}

// Tensor
int HPPS_TensorShape(Handle handle, uint32_t* out_dim, const uint32_t** out_data) {
  Tensor* tensor = reinterpret_cast<Tensor*>(handle);
  const auto& shape = tensor->shape();
  *out_dim = shape.size();
  *out_data = shape.data();
  return 0;
}

int HPPS_TensorType(Handle handle, uint8_t* out) {
  Tensor* tensor = reinterpret_cast<Tensor*>(handle);
  auto data_type = tensor->data_type();
  *out = data_type;
  return 0;
}

int HPPS_TensorData(Handle handle, void** out) {
  Tensor* tensor = reinterpret_cast<Tensor*>(handle);
  *out = tensor->mutable_blob()->data();
  return 0;
}

// Feeder start and Plan scheduling.
int HPPS_FeederStart(int thread_num) {
  Feeder::Get()->Start(thread_num);
  return 0;
}

int HPPS_FeederSchedule(Handle plan, int max_queue_size, Handle* iterator) {
  *iterator = Feeder::Get()->Schedule(reinterpret_cast<Plan*>(plan), max_queue_size);
  return 0;
}

// Plan maker creation
int HPPS_CreatePlanMaker(Handle* plan_maker) {
  *plan_maker = new PlanMaker();
  if (plan_maker == nullptr) {
    HPPS_SetLastErrorString("Create PlanMaker Failed");
    return -1;
  }
  return 0;
}

int HPPS_PlanMakerSetURI(Handle handle, int num, const char** uri) {
  PlanMaker* plan_maker = reinterpret_cast<PlanMaker*>(handle);
  std::vector<std::string> source;
  for (auto i = 0; i < num; ++i) {
    source.push_back(uri[i]);
  }
  plan_maker->set_uri(source);
  return 0;
}

int HPPS_PlanMakerSetEpoch(Handle handle, int epoch) {
  PlanMaker* plan_maker = reinterpret_cast<PlanMaker*>(handle);
  plan_maker->set_epoch(epoch);
  return 0;
}

int HPPS_PlanMakerSetBatchSize(Handle handle, int batch_size) {
  PlanMaker* plan_maker = reinterpret_cast<PlanMaker*>(handle);
  plan_maker->set_batch_size(batch_size);
  return 0;
}

int HPPS_PlanMakerMake(Handle handle, Handle* plan) {
  PlanMaker* plan_maker = reinterpret_cast<PlanMaker*>(handle);
  *plan = plan_maker->Make();
  return 0;
}

// Batch Iterator
int HPPS_BatchIteratorPop(Handle handle, Handle* out) {
  BlockingQueueIterator<std::unique_ptr<Batch>>* iterator =
      reinterpret_cast<BlockingQueueIterator<std::unique_ptr<Batch>>*>(handle);
  std::unique_ptr<Batch> batch;
  iterator->Pop(batch);
  *out = batch.release();
  return 0;
}

// Batch operation
int HPPS_BatchGetTensorFromKey(Handle handle, const char* key, Handle* tensor) {
  Batch* batch = reinterpret_cast<Batch*>(handle);
  *tensor = batch->Get(key);
  if (*tensor == nullptr) {
    HPPS_SetLastErrorString("Get tensor from key failed");
    return -1;
  }
  return 0;
}

int HPPS_BatchGetTensorFromIndex(Handle handle, int index, Handle* tensor) {
  Batch* batch = reinterpret_cast<Batch*>(handle);
  *tensor = batch->Get(index);
  if (*tensor == nullptr) {
    HPPS_SetLastErrorString("Get tensor from index failed");
    return -1;
  }
  return 0;
}

int HPPS_BatchDestroy(Handle handle) {
  Batch* batch = reinterpret_cast<Batch*>(handle);
  delete batch;
  return 0;
}
