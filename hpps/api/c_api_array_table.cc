/*
 * \file c_api_array_table.cc
 */
#include "hpps/api/c_api_array_table.h"

#include "hpps/api/c_api_error.h"
#include "hpps/common/random.h"
#include "hpps/common/tensor.h"
#include "hpps/common/types.h"
// the feeder's data types contains all the data types used in parameter server.
#include "hpps/frame/table_factory.h"
#include "hpps/table/array_table.h"

using namespace hpps;

int DeleteArrayTable(Handle handle) {
  WorkerTable* worker_table = reinterpret_cast<WorkerTable*>(handle);
  if (worker_table != nullptr) delete worker_table;
  return 0;
}

int ArrayTableGet(Handle handle, Handle tensor) {
  WorkerTable* worker_table = reinterpret_cast<WorkerTable*>(handle);
  if (worker_table != nullptr) {
    Tensor* tensor_ = reinterpret_cast<Tensor*>(tensor);
    VALUE_TYPE_SWITCH(tensor_->data_type(), DType, {
      ArrayWorker<DType>* worker = dynamic_cast<ArrayWorker<DType>*>(worker_table);
      worker->Get(reinterpret_cast<DType*>(tensor_->mutable_blob()->data()),
                  tensor_->mutable_blob()->size<DType>());
    });
  } else {
    HPPS_SetLastErrorString("Is not a worker node, should not call table get");
    return -1;
  }
  return 0;
}

int ArrayTableGetAsync(Handle handle, Handle tensor, int* id) {
  WorkerTable* worker_table = reinterpret_cast<WorkerTable*>(handle);
  if (worker_table != nullptr) {
    Tensor* tensor_ = reinterpret_cast<Tensor*>(tensor);
    VALUE_TYPE_SWITCH(tensor_->data_type(), DType, {
      ArrayWorker<DType>* worker = dynamic_cast<ArrayWorker<DType>*>(worker_table);
      *id = worker->GetAsync(reinterpret_cast<DType*>(tensor_->mutable_blob()->data()),
                             tensor_->mutable_blob()->size<DType>());
    });
  } else {
    HPPS_SetLastErrorString("Is not a worker node, should not call table get");
    return -1;
  }
  return 0;
}

int ArrayTableAdd(Handle handle, Handle grad) {
  WorkerTable* worker_table = reinterpret_cast<WorkerTable*>(handle);
  if (worker_table != nullptr) {
    Tensor* grad_ = reinterpret_cast<Tensor*>(grad);
    VALUE_TYPE_SWITCH(grad_->data_type(), DType, {
      ArrayWorker<DType>* worker = dynamic_cast<ArrayWorker<DType>*>(worker_table);
      worker->Add(reinterpret_cast<DType*>(grad_->mutable_blob()->data()),
                  grad_->mutable_blob()->size<DType>());
    });
  } else {
    HPPS_SetLastErrorString("Is not a worker node, should not call table add");
    return -1;
  } 
  return 0;
}

int ArrayTableAddAsync(Handle handle, Handle grad, int* id) {
  WorkerTable* worker_table = reinterpret_cast<WorkerTable*>(handle);
  if (worker_table != nullptr) {
    Tensor* grad_ = reinterpret_cast<Tensor*>(grad);
    VALUE_TYPE_SWITCH(grad_->data_type(), DType, {
      ArrayWorker<DType>* worker = dynamic_cast<ArrayWorker<DType>*>(worker_table);
      *id = worker->AddAsync(reinterpret_cast<DType*>(grad_->mutable_blob()->data()),
                             grad_->mutable_blob()->size<DType>());
    });
  } else {
    HPPS_SetLastErrorString("Is not a worker node, should not call table add");
    return -1;
  } 
  return 0;
}

int CreateArrayTable(size_t size,
                     int type,
                     int num,
                     const char** key,
                     const char** value,
                     Handle* out) {
  std::map<std::string, std::string> kwargs;
  for (auto i = 0; i < num; ++i) {
    kwargs[key[i]] = value[i];
  }
  VALUE_TYPE_SWITCH(type, DType, {
    ArrayTableOption<DType> array_table_option(size);
    array_table_option.random_option = CreateRandomOption(kwargs); 
    *out = table_factory::CreateTable(array_table_option);
  });
  return 0;
}
