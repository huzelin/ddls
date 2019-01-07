/*!
 * \file updater.h
 * \brief The updater
 */
#pragma once

#include <cstring>
#include <sstream>

#include "hpps/frame/zoo.h"

namespace hpps {

struct AddOption {
 public:
  static const int kGradUpdateType = 0;
  static const int kParamUpdateType = 1;

  AddOption(){
    data_[0].i = Zoo::Get()->worker_rank(); 
    set_learning_rate(0.001f);
    set_momentum(0.0f);
    set_rho(0.1f);
    set_lambda(0.1f);
    set_type(kGradUpdateType);
    set_beta1(0.9);
    set_beta2(0.999);
    set_eps(1e-8f);
  }

  AddOption(const char* data, size_t size) {
    CopyFrom(data, size);
  }

  int worker_id() const { return data_[0].i; }
  void set_worker_id(int worker_id) { data_[0].i = worker_id; }
  float learning_rate() const { return data_[1].f; }
  void set_learning_rate(float lr) { data_[1].f = lr; }
  int type() const { return data_[2].i; }
  void set_type(int type) { data_[2].i = type; }

  float momentum() const { return data_[3].f; }
  void set_momentum(float momentum) { data_[3].f = momentum; }
  // rho and lambda are two coefficient used by other algorithms
  float rho() const { return data_[4].f; }
  void set_rho(float rho) { data_[4].f = rho; }
  float lambda() const { return data_[5].f; }
  void set_lambda(float lambda) { data_[5].f = lambda; }
  
  // adam algorithm
  float beta1() const { return data_[3].f; }
  void set_beta1(float beta1) { data_[3].f = beta1; }
  float beta2() const { return data_[4].f; }
  void set_beta2(float beta2) { data_[4].f = beta2; }
  float eps() const { return data_[5].f; }
  void set_eps(float eps) { data_[5].f = eps; }

  std::string toString(){
    std::stringstream  ss;
    ss << "AddOption " << worker_id() << " " << momentum() << " "
      << learning_rate() << " " << rho() << " " << lambda() << std::endl;

    return ss.str();
  }

  const char* data() const { return reinterpret_cast<const char*>(&data_[0]); }
  size_t size() const { return kSize * sizeof(InternalType); }
  void CopyFrom(const char* data, size_t size) { 
    memcpy(data_, data, size);
  }
 
 private:
  static const size_t kSize = 6;
  // Option can be either int type or float, 
  // to make it easy to serialize and deserialize
  union InternalType{
    int i;
    float f;
  };

  // May add other option for future potential update algorithm
  // 0: src worker id
  // 1: learning rate
  // 2: momentum
  // 3: rho
  // 4: lambda
  // ...
  InternalType data_[kSize];
};

struct GetOption {
 public:
  GetOption(){
    data_[0].i = Zoo::Get()->worker_rank();
  }

  GetOption(const char* data, size_t size) {
    CopyFrom(data, size);
  }

  int worker_id() const { return data_[0].i; }
  void set_worker_id(int worker_id) { data_[0].i = worker_id; }

  std::string toString(){
    std::stringstream  ss;
    ss << "AddOption " << worker_id() << std::endl;
    return ss.str();
  }


  const char* data() const { return reinterpret_cast<const char*>(&data_[0]); }
  size_t size() const { return kSize * sizeof(InternalType); }
  void CopyFrom(const char* data, size_t size) {
    memcpy(data_, data, size);
  }
 
 private:
  static const size_t kSize = 1;
  // Option can be either int type or float, 
  // to make it easy to serialize and deserialize
  union InternalType{
    int i;
    float f;
  };

  // 0: src worker id
  // ...
  InternalType data_[kSize];
};


template <typename T>
class Updater {
 public:
  virtual ~Updater() = default;
  // The updater will update the data with delta in following way
  // Add delta[0 : num_element) to data[offset : offset+num_element)
  // for index in range(0, num_element):
  //    Update data[index + offset] with delta[index], option, and the updater member
  // This is mainly for model sparse update consideration
  virtual void Update(size_t num_element, T* data, T* delta, 
                      AddOption* option = nullptr, size_t offset = 0);

  // The updater will access the data to out_data in following way 
  //   Get data[offset : offset + num_element) to blob_data[0 : num_element)
  virtual void Access(size_t num_element, T* data, T* blob_data,
                      size_t offset = 0, AddOption* option = nullptr);

  // Factory method to get the updater
  static Updater<T>* GetUpdater(size_t size = 0);
};

#define HPPS_INSTANTIATE_CLASS_WITH_REAL_TYPE(classname) \
  template class classname<float>;                     

#define HPPS_INSTANTIATE_CLASS_WITH_BASE_TYPE(classname) \
  HPPS_INSTANTIATE_CLASS_WITH_REAL_TYPE(classname)       \
  template class classname<int>;                       

}  // namespace hpps
