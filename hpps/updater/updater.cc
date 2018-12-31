/*!
 * \file updater.cc
 * \brief The updater
 */
#include "hpps/updater/updater.h"

#include "hpps/updater/adagrad_updater.h"
#include "hpps/updater/momentum_updater.h"
#ifdef ENABLE_DCASGD
#include "hpps/updater/dcasgd/dcasgd_updater.h"
#include "hpps/updater/dcasgd/dcasgda_updater.h"
#endif
#include "hpps/updater/sgd_updater.h"
#include "hpps/common/configure.h"
#include "hpps/common/log.h"


namespace hpps {

HPPS_DEFINE_string(updater_type, "sgd", "hpps server updater type");
HPPS_DEFINE_int(omp_threads, 4 , "#theads used by openMP for updater");

template <typename T>
void Updater<T>::Update(size_t num_element, T* data, T* delta,
                        AddOption*, size_t offset) {
  // parallelism with openMP
  #pragma omp parallel for schedule(static) num_threads(HPPS_CONFIG_omp_threads)
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

// Gradient-based updater in only for numerical table
// For simple int table, just using simple updater
template<>
Updater<int>* Updater<int>::GetUpdater(size_t) {
  return new Updater<int>();
}

template <typename T>
Updater<T>* Updater<T>::GetUpdater(size_t size) {
  std::string type = HPPS_CONFIG_updater_type;
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
