/*!
 * \file sgd_updater.h
 * \brief The SGD updater
 */
#pragma once

#include "hpps/updater/updater.h"

namespace hpps {

template <typename T>
class SGDUpdater : public Updater<T> {
public:
  explicit SGDUpdater(size_t){
    Log::Debug("[SGDUpdater] Init. \n");
  }
  void Update(size_t num_element, T* data, T* delta,
              AddOption*, size_t offset) override {
    for (size_t index = 0; index < num_element; ++index) {
      data[index + offset] -= delta[index];
    }
  }

  void Access(size_t num_element, T* data, T* blob_data,
              size_t offset, AddOption*) override{
    memcpy(blob_data, data + offset, sizeof(T) * num_element);
  }

  ~SGDUpdater(){}
};

}  // namespace hpps
