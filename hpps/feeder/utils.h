/*!
 * \file utils.h
 * \brief The utils of feeder
 */
#pragma once

#include "hpps/feeder/tensor.h"

namespace hpps {

inline Tensor* Num2Indices(Tensor* num_tensor) {
  auto data_type = num_tensor->data_type();

  ID_TYPE_SWITCH(data_type, DType, {
    auto blob = num_tensor->mutable_blob();
    DType* addr = reinterpret_cast<DType*>(blob->data());
    tensor_dim_t size = 0;
    for (auto i = 0; i < num_tensor->size(); ++i) {
      size += addr[i];
    }
    Tensor* indices = new Tensor({ size }, data_type);
    uint32_t* int_addr = reinterpret_cast<uint32_t*>(indices->mutable_blob()->data());
    for (auto i = 0; i < num_tensor->size(); ++i) {
      auto count = addr[i];
      for (auto j = 0; j < count; ++j) {
        *int_addr++ = i;
      }
    }
    return indices; 
  });
  return nullptr;
}

}  // namespce hpps
