/*
 * \file c_api_tensor.cc
 */
#include "hpps/api/c_api_tensor.h"

#include <vector>

#include "hpps/api/c_api_error.h"
#include "hpps/common/tensor.h"

using namespace hpps;

// Tensor
int HPPS_TensorCreate(uint32_t dim, const uint32_t* shape, uint32_t type, Handle* out) {
  std::vector<tensor_dim_t> shp;
  for (auto i = 0; i < dim; ++i) {
    shp.push_back(shape[i]);
  }
  tensor_data_type_t data_type = type;
  auto tensor = new Tensor(shp, type);
  *out = tensor;
  return 0;
}

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

int HPPS_TensorLoadData(Handle handle, size_t offset, void* data, size_t size) {
  Tensor* tensor = reinterpret_cast<Tensor*>(handle);
  auto data_type_size = TensorDataTypeSize(tensor->data_type());
  memcpy(tensor->mutable_blob()->data() + data_type_size * offset,
         data,
         size * data_type_size);
  return 0;
}

int HPPS_TensorExportData(Handle handle, void* data) {
  Tensor* tensor = reinterpret_cast<Tensor*>(handle);
  memcpy(data,
         tensor->mutable_blob()->data(),
         tensor->mutable_blob()->size());
  return 0;
}

int HPPS_TensorDestroy(Handle handle) {
  Tensor* tensor = reinterpret_cast<Tensor*>(handle);
  delete tensor;
  return 0;
}
