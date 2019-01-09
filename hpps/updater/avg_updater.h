/*!
 * \file avg_updater.h
 * \brief The window average updater
 */
#pragma once

#include "hpps/common/log.h"
#include "hpps/updater/updater.h"

namespace hpps {

template <typename T>
class AvgUpdater : public Updater<T> {
 public:
  explicit AvgUpdater(size_t size) : size_(size) {
    LOG_DEBUG("[AvgUpdater] Init. ");
    sum_.resize(size_);
    accu_num_ = 0;
  }

  void Update(size_t num_element, T* data, T* delta,
              AddOption* option, size_t offset) override {
    for (auto i = 0; i < num_element; ++i) {
      sum_[i] += delta[i];
    }
    if (++accu_num_ == Zoo::Get()->num_workers()) {
      for (auto i = 0; i < num_element; ++i) {
        data[offset + i] = sum_[i] / accu_num_;
      }
      for (auto& item : sum_) item = 0;
      accu_num_ = 0;
    }
  }

  void Access(size_t num_element, T* data, T* blob_data,
              size_t offset, AddOption*) override {
    memcpy(blob_data, data + offset, sizeof(T) * num_element);
  }

  ~AvgUpdater() { }

 protected:
  size_t accu_num_;
  std::vector<T> sum_;
  size_t size_;
};

}  // namespace hpps
