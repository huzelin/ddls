/*
 * \file c_api_feeder.cc
 */

#include "hpps/api/c_api_feeder.h"

#include <vector>

#include "hpps/api/c_api_error.h"
#include "hpps/common/tensor.h"
#include "hpps/common/thread_local.h"
#include "hpps/feeder/feeder.h"
#include "hpps/feeder/record_io.h"
#include "hpps/feeder/utils.h"

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

int HPPS_RecordIOWriteHeader(Handle handle, int num, const char** name, const int* type, const int* level, const int* is_aux_number) {
  RecordIO* record_io = reinterpret_cast<RecordIO*>(handle);
  std::unordered_map<std::string, RecordIO::ItemConfig> tensor_meta;
  for (auto i = 0; i < num; ++i) {
    RecordIO::ItemConfig item_config(type[i]);
    item_config.level = level[i];
    item_config.is_aux_number = is_aux_number[i];
    tensor_meta[name[i]] = item_config;
  }
  record_io->WriteHeader(tensor_meta);
  return 0;
}

int HPPS_RecordIOWriteSample(Handle handle, int num, const char** name, Handle* tensor) {
  RecordIO* record_io = reinterpret_cast<RecordIO*>(handle);
  std::unordered_map<std::string, Tensor*> sample;
  for (auto i = 0; i < num; ++i) {
    sample[name[i]] = reinterpret_cast<Tensor*>(tensor[i]);
  }
  record_io->WriteSample(sample);
  return 0;
}

int HPPS_RecordIOWriteFinalize(Handle handle) {
  RecordIO* record_io = reinterpret_cast<RecordIO*>(handle);
  record_io->WriteFinalize();
  return 0;
}

int HPPS_RecordIODestroy(Handle handle) {
  RecordIO* record_io = reinterpret_cast<RecordIO*>(handle);
  delete record_io;
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

int HPPS_FeederStop() {
  Feeder::Get()->Stop();
  return 0;
}

// Plan maker creation
int HPPS_PlanMakerCreate(Handle* plan_maker) {
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
static const int kMaxBufSize = 8192;

struct StringArrayEntry {
  const char* buf[kMaxBufSize];
};
typedef hpps::ThreadLocalStore<StringArrayEntry> ThreadLocalStringArrayStore;

int HPPS_BatchGetKeys(Handle handle, int* out_name_size, const char*** out_names) {
  Batch* batch = reinterpret_cast<Batch*>(handle);
  if (batch == nullptr) {
    HPPS_SetLastErrorString("Batch is null");
    return -1;
  }
  const auto& names = batch->names();
  auto& buf = ThreadLocalStringArrayStore::Get()->buf;
  *out_name_size = names.size();

  for (auto i = 0; i < *out_name_size; ++i) {
    buf[i] = names[i].c_str();
  }
  *out_names = buf;
  return 0;
}

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

// Utils operation
int HPPS_Num2Indices(Handle num_handle, Handle* indices) {
  Tensor* tensor = reinterpret_cast<Tensor*>(num_handle);
  *indices = Num2Indices(tensor);
  return 0;
}

int HPPS_Id2UniqId(Handle id_handle, Handle* local_id_handle, Handle* uniq_id_handle) {
  Tensor* tensor = reinterpret_cast<Tensor*>(id_handle);
  auto result = Id2UniqId(tensor);
  *uniq_id_handle = result[0];
  *local_id_handle = result[1];
  return 0;
}

int HPPS_AddIndicesTensor(Handle handle, int num, const char** names) {
  Batch* batch = reinterpret_cast<Batch*>(handle);
  if (batch == nullptr) {
    HPPS_SetLastErrorString("Batch is null");
    return -1;
  }
  std::vector<std::string> vec_names;
  for (int i = 0; i < num; ++i) vec_names.push_back(names[i]);
  AddIndicesTensor(batch, vec_names);
  return 0;
}

int HPPS_AddUniqIdTensor(Handle handle, int num, const char** names) {
  Batch* batch = reinterpret_cast<Batch*>(handle);
  if (batch == nullptr) {
    HPPS_SetLastErrorString("Batch is null");
    return -1;
  }
  std::vector<std::string> vec_names;
  for (int i = 0; i < num; ++i) vec_names.push_back(names[i]);
  AddUniqIdTensor(batch, vec_names);
  return 0;
}
