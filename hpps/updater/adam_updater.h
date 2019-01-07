/*!
 * \file adam_updater.h
 * \brief The adam updater
 */
#pragma once

#include "hpps/common/log.h"
#include "hpps/updater/updater.h"

namespace hpps {

template <typename T>
class AdamUpdater : public Updater<T> {
 public:
  explicit AdamUpdater(size_t size) : size_(size) {
    LOG_DEBUG("[AdamUpdater] Init. ");
    mom1_.resize(size_);
    mom2_.resize(size_);
  }

  void Update(size_t num_element, T* data, T* delta,
              AddOption* option, size_t offset) override {
    auto beta1 = option->beta1();
    auto beta2 = option->beta2();
    auto eps = option->eps();
    auto lr = option->learning_rate();

    for (size_t index = 0; index < num_element; ++index) {
      T var_mom1, var_mom2;
      var_mom1 = beta1 * mom1_[offset + index] + (1.0 - beta1) * delta[index];
      var_mom2 = beta2 * mom2_[offset + index] + (1.0 - beta2) * delta[index] * delta[index];
      data[offset + index] -= lr * var_mom1 / (sqrtf(var_mom2) + eps);
      mom1_[offset + index] = var_mom1;
      mom2_[offset + index] = var_mom2;
      data[offset + index] -= delta[index] * lr;
    }
  }

  void Access(size_t num_element, T* data, T* blob_data,
              size_t offset, AddOption*) override{
    memcpy(blob_data, data + offset, sizeof(T) * num_element);
  }

  ~AdamUpdater(){}

 protected:
  std::vector<T> mom1_, mom2_;
  size_t size_;
};

}  // namespace hpps
