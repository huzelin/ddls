/*
 * \file c_api_kv_table.cc
 */
#include "hpps/api/c_api_kv_table.h"

#include "hpps/api/c_api_error.h"
#include "hpps/common/random.h"
#include "hpps/common/tensor.h"
#include "hpps/common/types.h"
// the feeder's data types contains all the data types used in parameter server.
#include "hpps/frame/table_factory.h"
#include "hpps/table/kv_table.h"
#include "hpps/updater/updater.h"

using namespace hpps;

int DeleteKVTable(Handle handle) {
  WorkerTable* worker_table = reinterpret_cast<WorkerTable*>(handle);
  if (worker_table != nullptr) delete worker_table;
  return 0;
}

int KVTableGet(Handle handle, Handle key, Handle tensor) {
  WorkerTable* worker_table = reinterpret_cast<WorkerTable*>(handle);
  if (worker_table != nullptr) {
    Tensor* k = reinterpret_cast<Tensor*>(key);
    Tensor* v = reinterpret_cast<Tensor*>(tensor);
    ID_TYPE_SWITCH(k->data_type(), ID_TYPE, {
      VALUE_TYPE_SWITCH(v->data_type(), VALUE_TYPE, {
        KVWorkerTable<ID_TYPE, VALUE_TYPE>* worker = dynamic_cast<KVWorkerTable<ID_TYPE, VALUE_TYPE>*>(worker_table);
        worker->Get(reinterpret_cast<ID_TYPE*>(k->mutable_blob()->data()), k->size(), v);
      });            
    });
  } else {
    HPPS_SetLastErrorString("Is not a worker node, should not call table get");
    return -1;
  }
  return 0;
}

int KVTableGetAsync(Handle handle, Handle key, Handle tensor, int* id) {
  WorkerTable* worker_table = reinterpret_cast<WorkerTable*>(handle);
  if (worker_table != nullptr) {
    Tensor* k = reinterpret_cast<Tensor*>(key);
    Tensor* v = reinterpret_cast<Tensor*>(tensor);
    ID_TYPE_SWITCH(k->data_type(), ID_TYPE, {
      VALUE_TYPE_SWITCH(v->data_type(), VALUE_TYPE, {
        KVWorkerTable<ID_TYPE, VALUE_TYPE>* worker = dynamic_cast<KVWorkerTable<ID_TYPE, VALUE_TYPE>*>(worker_table);
        *id = worker->GetAsync(reinterpret_cast<ID_TYPE*>(k->mutable_blob()->data()), k->size());
      });            
    });
  } else {
    HPPS_SetLastErrorString("Is not a worker node, should not call table get");
    return -1;
  }
  return 0;
}

static AddOption CreateAddOption(int num, const char** key, const char** value) {
  std::map<std::string, std::string> kwargs;
  for (auto i = 0; i < num; ++i) {
    kwargs[key[i]] = value[i];
  }
  auto add_option = CreateAddOption(kwargs);
  return add_option;
}

int KVTableAdd(Handle handle, Handle key, Handle grad, int num, const char** option_key, const char** option_value) {
  auto add_option = CreateAddOption(num, option_key, option_value);

  WorkerTable* worker_table = reinterpret_cast<WorkerTable*>(handle);
  if (worker_table != nullptr) {
    Tensor* k = reinterpret_cast<Tensor*>(key);
    Tensor* v = reinterpret_cast<Tensor*>(grad);
    ID_TYPE_SWITCH(k->data_type(), ID_TYPE, {
      VALUE_TYPE_SWITCH(v->data_type(), VALUE_TYPE, {
        KVWorkerTable<ID_TYPE, VALUE_TYPE>* worker = dynamic_cast<KVWorkerTable<ID_TYPE, VALUE_TYPE>*>(worker_table);
        worker->Add(reinterpret_cast<ID_TYPE*>(k->mutable_blob()->data()),
                    k->size(),
                    reinterpret_cast<VALUE_TYPE*>(v->mutable_blob()->data()),
                    v->size(),
                    &add_option);        
      });               
    });
  } else {
    HPPS_SetLastErrorString("Is not a worker node, should not call table add");
    return -1;
  } 
  return 0;
}

int KVTableAddAsync(Handle handle, Handle key, Handle grad, int* id,
                    int num, const char** option_key, const char** option_value) {
  auto add_option = CreateAddOption(num, option_key, option_value);

  WorkerTable* worker_table = reinterpret_cast<WorkerTable*>(handle);
  if (worker_table != nullptr) {
    Tensor* k = reinterpret_cast<Tensor*>(key);
    Tensor* v = reinterpret_cast<Tensor*>(grad);
    ID_TYPE_SWITCH(k->data_type(), ID_TYPE, {
      VALUE_TYPE_SWITCH(v->data_type(), VALUE_TYPE, {
        KVWorkerTable<ID_TYPE, VALUE_TYPE>* worker = dynamic_cast<KVWorkerTable<ID_TYPE, VALUE_TYPE>*>(worker_table);
        *id = worker->AddAsync(reinterpret_cast<ID_TYPE*>(k->mutable_blob()->data()),
                               k->size(),
                               reinterpret_cast<VALUE_TYPE*>(v->mutable_blob()->data()),
                               v->size(),
                               &add_option);
      });               
    });
  } else {
    HPPS_SetLastErrorString("Is not a worker node, should not call table add");
    return -1;
  } 
  return 0;
}

int CreateKVTable(const char* solver,
                  const char* ps_mode,
                  size_t capacity,
                  int value_len,
                  int key_type,
                  int value_type,
                  int num,
                  const char** key,
                  const char** value,
                  Handle* out) {
  std::map<std::string, std::string> kwargs;
  for (auto i = 0; i < num; ++i) {
    kwargs[key[i]] = value[i];
  }
  ID_TYPE_SWITCH(key_type, ID_TYPE, {
    VALUE_TYPE_SWITCH(value_type, VALUE_TYPE, {
      KVTableOption<ID_TYPE, VALUE_TYPE> kv_table_option(capacity, value_len, solver, ps_mode);
      kv_table_option.random_option = CreateRandomOption(kwargs); 
      *out = table_factory::CreateTable(kv_table_option);
    });
  });
  return 0;
}
