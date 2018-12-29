/*
 * \file random.h 
 * \brief The random
 */
#pragma once

#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>

namespace hpps {

// The random algorithm type
enum RandomAlgoType {
  kGaussian = 0,
  kUniform,
  kAssign,
};

struct RandomOption {
  RandomOption() {
    data_.resize(12);
    set_algorithm(RandomAlgoType::kGaussian);
    set_seed(0);
    set_min(0);
    set_max(1.0);
    set_mu(0);
    set_sigma(0.01);
  }
  void set_algorithm(RandomAlgoType ra) { data_[0].i = ra; }
  RandomAlgoType algorithm() const { return static_cast<RandomAlgoType>(data_[0].i); }

  void set_mu(float mu) { data_[1].f = mu; }
  float mu() const { return data_[1].f; }

  void set_sigma(float sigma) { data_[2].f = sigma; }
  float sigma() const { return data_[2].f; }

  void set_assigned_value(float value) { data_[3].f = value; }
  float assigned_value() const { return data_[3].f; }

  void set_seed(int seed) { data_[4].i = seed; }
  int seed() const { return data_[4].i; }

  void set_min(float min) { data_[5].f = min; }
  float min() const { return data_[5].f; }

  void set_max(float max) { data_[6].f = max; }
  float max() const { return data_[6].f; }

 protected:
  union InternalType {
    int i;
    float f;
  };
  std::vector<InternalType> data_;
  // 0: algorithm
  // 1: mu
  // 2: sigma
  // 3: assigned value
  // 4: seed 
  // 5: min
  // 6: max
};

// create random option from kwargs.
RandomOption CreateRandomOption(const std::map<std::string, std::string>& kwargs);

template <typename DType>
class Random {
 public:
  Random();
  explicit Random(const RandomOption& random_option);
  virtual ~Random() { }

  void SetRandomOption(const RandomOption& random_option);
  void Gen(DType* dptr, size_t size);

 protected:
  void GenBuffer();
  void GenAssignBuffer();
  void GenUniformBuffer();
  void GenGaussianBuffer();

  RandomOption random_option_;
  std::default_random_engine rnd_engine_;

  std::vector<DType> buffer_;
  size_t buffer_remaining_;
  static const size_t kRandomBufferSize = 1024;
};

}  // namespace hpps
