/*
 * \file dcasgd_updater.h
 * \brief See https://arxiv.org/abs/1609.08326 for the details.
 */
#pragma once

#include "hpps/updater/updater.h"

#include <vector>
#include <cmath>

namespace hpps {

template <typename T>
class DCASGDUpdater : public Updater<T> {
 public:
  explicit DCASGDUpdater(size_t size) :
      size_(size){
	  LOG_DEBUG("[DC-ASGDUpdater] Init. \n");
		shadow_copies_.resize(MV_NumWorkers(), std::vector<T>(size_));
	}
  
  void Update(size_t num_element, T*data, T*delta,
              AddOption* option, size_t offset) override {
    for (size_t index = 0; index < num_element; ++index) {
      T g = delta[index] / option->learning_rate();
      data[index + offset] -= option->learning_rate() *
          (g + option->lambda() *	g * g *
           (data[index + offset] - shadow_copies_[option->worker_id()][index + offset]));
      // caching each worker's latest version of parameter
			shadow_copies_[option->worker_id()][index + offset] = data[index + offset];
		}
	}

	~DCASGDUpdater() {
    shadow_copies_.clear();
	}
 
 protected:
  std::vector< std::vector<T>> shadow_copies_;
  size_t size_;
};

}  // namespace hpps
