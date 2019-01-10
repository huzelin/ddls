/*!
 * \file updater.cc
 * \brief The updater
 */
#include "hpps/updater/updater.h"

#include "hpps/updater/adam_updater.h"
#include "hpps/updater/adagrad_updater.h"
#include "hpps/updater/avg_updater.h"
#include "hpps/updater/momentum_updater.h"
#ifdef ENABLE_DCASGD
#include "hpps/updater/dcasgd/dcasgd_updater.h"
#include "hpps/updater/dcasgd/dcasgda_updater.h"
#endif
#include "hpps/updater/sgd_updater.h"
#include "hpps/common/configure.h"
#include "hpps/common/log.h"

namespace {
std::string kStrLr = "lr";
}  // namespace

namespace hpps {

AddOption CreateAddOption(const std::map<std::string, std::string>& kwargs) {
  AddOption add_option;
  auto lr_iter = kwargs.find(kStrLr);
  if (lr_iter != kwargs.end()) {
    std::string val = lr_iter->second;
    add_option.set_learning_rate(std::stof(val.c_str()));
  }
  return add_option;
}

HPPS_DEFINE_string(updater_type, "adam", "hpps server updater type");

template <typename T>
void Updater<T>::Update(size_t num_element, T* data, T* delta,
                        AddOption*, size_t offset) {
  for (int i = 0; i < num_element; ++i) {
    data[i + offset] += delta[i];
  }
}

template <typename T>
void Updater<T>::Access(size_t num_element, T* data, T* blob_data,
  size_t offset , AddOption*) {
  // copy data from data to blob
  memcpy(blob_data, data + offset, num_element * sizeof(T));
}

template <typename T>
Updater<T>* Updater<T>::GetUpdater(size_t size, const std::string& solver) {
  std::string type = HPPS_CONFIG_updater_type;
  if (!solver.empty()) type = solver;  // If user defined is not empty.

  if (type == "adam") return new AdamUpdater<T>(size);
  if (type == "avg") return new AvgUpdater<T>(size);
  if (type == "sgd") return new SGDUpdater<T>(size);
  if (type == "adagrad") return new AdaGradUpdater<T>(size);
  if (type == "momentum_sgd") return new MomentumUpdater<T>(size);
#ifdef ENABLE_DCASGD
  if (type == "dcasgd") return new DCASGDUpdater<T>(size);
  if (type == "dcasgda") return new DCASGDAUpdater<T>(size);
#endif
  // Default: simple updater
  return new Updater<T>();
}

HPPS_INSTANTIATE_CLASS_WITH_BASE_TYPE(Updater);

}  // namespace hpps
