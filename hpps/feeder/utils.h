/*!
 * \file utils.h
 * \brief The utils of feeder
 */
#pragma once

#include "hpps/common/tensor.h"
#include "hpps/feeder/batch.h"

#include <unordered_set>

namespace hpps {

// num2 to indicator
inline Tensor* Num2Indices(Tensor* num_tensor) {
  auto data_type = num_tensor->data_type();
  CHECK(data_type == kUInt32);

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

inline void AddIndicesTensor(Batch* batch, const std::vector<std::string>& names) {
  static const std::string kSuffix = "_indices";
  for (const auto& name : names) {
    auto tensor = batch->Get(name);
    auto indices = Num2Indices(tensor);
    auto new_name = name + kSuffix;
    batch->Insert(new_name, indices);
  }
}

// Convert ID to local id and uniq id
inline std::vector<Tensor*> Id2UniqId(Tensor* id) {
  CHECK(id->data_type() == kUInt64);
  std::unordered_map<uint64_t, uint32_t> uniqid_2_localid;
  std::vector<uint64_t> uniq_id;

  // Step1: create local id tensor
  tensor_dim_t local_id_size = id->mutable_blob()->size<uint64_t>();
  Tensor* local_id_tensor = new Tensor({ local_id_size }, kUInt32);

  // Step2: calculate uniq id
  for (auto i = 0; i < id->mutable_blob()->size<uint64_t>(); ++i) {
    auto current_id = id->mutable_blob()->As<uint64_t>(i);
    auto iter = uniqid_2_localid.find(current_id);
    if (iter == uniqid_2_localid.end()) {
      auto new_id = uniq_id.size();
      local_id_tensor->mutable_blob()->As<uint32_t>(i) = new_id;
      uniq_id.push_back(current_id);
      uniqid_2_localid[current_id] = new_id;
    } else {
      local_id_tensor->mutable_blob()->As<uint32_t>(i) = iter->second;
    }
  }

  // Step3: generate uniqid tensor
  tensor_dim_t uniq_id_size = uniq_id.size();
  Tensor* uniqid_tensor = new Tensor({ uniq_id_size }, kUInt64);
  for (auto i = 0; i < uniq_id_size; ++i) {
    uniqid_tensor->mutable_blob()->As<uint64_t>(i) = uniq_id[i];
  }

  return { uniqid_tensor, local_id_tensor };
}

inline void AddUniqIdTensor(Batch* batch, const std::vector<std::string>& names) {
  static const std::vector<std::string> kSuffix = { "_uniqid", "_localid" };
  for (const auto& name : names) {
    auto tensor = batch->Get(name);
    auto expand_tensors = Id2UniqId(tensor);
    for (auto i = 0; i < expand_tensors.size(); ++i) {
      std::string new_name = name + kSuffix[i];
      batch->Insert(new_name, expand_tensors[i]);
    }
  }  // for (const auto& name : names) {
}

}  // namespce hpps
