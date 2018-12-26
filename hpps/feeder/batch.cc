/*!
 * \file batch.cc
 * \brief The batch definition
 */
#include "hpps/feeder/batch.h"

#include "hpps/common/log.h"

namespace hpps {

Batch::Batch(const std::vector<std::string>& names) {
  for (auto i = 0; i < names.size(); ++i) {
    index_[names[i]] = i;
  }
  tensors_.resize(names.size(), nullptr);
}

Batch::~Batch() {
  for (auto& tensor : tensors_) {
    delete tensor;
  }
}

Tensor* Batch::Get(const std::string& key) {
  const auto& iter = index_.find(key);
  if (iter == index_.end()) return nullptr;
  CHECK(iter->second >= 0 && iter->second < tensors_.size());
  return tensors_[iter->second];
}

Tensor* Batch::Get(int index) {
  CHECK(index >= 0 && index < tensors_.size());
  return tensors_[index];
}

void Batch::Set(int index, Tensor* tensor) {
  CHECK(index >= 0 && index < tensors_.size());
  tensors_[index] = tensor;
}

std::vector<std::string> Batch::Keys() {
  std::vector<std::string> ret;
  for (const auto& iter : index_) {
    ret.push_back(iter.first);
  }
  return ret;
}

}  // namespace hpps
