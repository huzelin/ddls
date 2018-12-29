/*!
 * \file adagrad_updater.h
 * \brief adagrad optimizer
 */
#pragma once

#include "hpps/updater/updater.h"
#include "hpps/common/log.h"
#include "hpps/frame/zoo.h"

#include <vector>
#include <cmath>
#include <cstdint>

namespace hpps {

template <typename T>
class AdaGradUpdater : public Updater<T> {
 public:
  explicit AdaGradUpdater(size_t size):
    e(1e-6f), size_(size) {  
    historic_g_sqr_.resize(Zoo::Get()->num_workers(), std::vector<T>(size_));
    LOG_DEBUG("[AdaGradUpdater] Init with size = %d, e = %f. historic_size = %d\n", size_, e, historic_g_sqr_.size());
  }

  void Update(size_t num_element, T* data, T* delta, 
              AddOption* option, size_t offset) override {
    auto g_sqr_data_ = historic_g_sqr_.at(option->worker_id());
    for (size_t index = 0; index < num_element; ++index) {
      g_sqr_data_[index + offset] -=
        delta[index] * delta[index] / option->learning_rate() / 
        option->learning_rate();

      data[index + offset] -= option->rho() /
        std::sqrt(g_sqr_data_[index + offset] + e) *
        delta[index] / option->learning_rate();
    }
  }
 
 private:
  float QuakeRsqrt(float number){
    float x = number * 0.5f, y = number;
    std::uint32_t i;
    std::memcpy(&i, &y, sizeof(float));
    i = 0x5f3759df - (i >> 1);
    std::memcpy(&y, &i, sizeof(float));
    return y * (1.5f - (x * y * y));
  }
 
 protected:
  std::vector< std::vector<T>> historic_g_sqr_;
  float e;
  size_t size_;
};

}  // namespace hpps
