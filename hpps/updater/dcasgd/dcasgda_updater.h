/*
 * \file dcasgda_updater.h
 * \brief See https://arxiv.org/abs/1609.08326 for the details.
 */
#pragma once

#include "hpps/updater/updater.h"

#include <vector>
#include <cmath>

namespace hpps {

template <typename T>
class DCASGDAUpdater : public Updater<T> {
 public:
  explicit DCASGDAUpdater(size_t size) :
      size_(size) {
    LOG_DEBUG("[DC-ASGD-A-Updater] Init.");
		shadow_copies_.resize(Zoo::Get()->num_workers(), std::vector<T>(size_));
		mean_square_.resize(Zoo::Get()->num_workers(), std::vector<T>(size_, 0.));
	}

	void Update(size_t num_element, T*data, T*delta,
              AddOption* option, size_t offset) override {
		float e = 1e-7;
		for (size_t index = 0; index < num_element; ++index) {
      T g = delta[index] / option->learning_rate();

      mean_square_[option->worker_id()][index + offset] *= option->momentum();
      mean_square_[option->worker_id()][index + offset] += (1 - option->momentum()) * g * g;
      data[index + offset] -= option->learning_rate() *
          (g + option->lambda() / sqrt(mean_square_[option->worker_id()][index + offset] + e)*
           g * g *
           (data[index + offset] - shadow_copies_[option->worker_id()][index + offset]));
					
      // caching each worker's latest version of parameter
      shadow_copies_[option->worker_id()][index + offset] = data[index + offset];
    }
  }
  
  ~DCASGDAUpdater() {
    shadow_copies_.clear();
		mean_square_.clear();
	}
 
 protected:
  std::vector< std::vector<T>> shadow_copies_;
  std::vector< std::vector<T>> mean_square_;
  size_t size_;
};

}  // namespace hpps
