/*
 * \file random.cc 
 * \brief The random module
 */
#include "hpps/common/random.h"

#include <string.h>

#include "hpps/common/log.h"

namespace {
std::string kStrAlgorithm = "algo";
std::string kStrMu = "mu";
std::string kStrSigma = "sigma";
std::string kStrAssignedValue = "assigned_value";
std::string kStrSeed = "seed";
std::map<std::string, hpps::RandomAlgoType> kStr2AlgoType = { 
  { "gaussian" , hpps::RandomAlgoType::kGaussian },
  { "uniform"  , hpps::RandomAlgoType::kUniform },
  { "assign"   , hpps::RandomAlgoType::kAssign },
};
}  // namespace

namespace hpps {

RandomOption CreateRandomOption(const std::map<std::string, std::string>& kwargs) {
  RandomOption random_option;

  auto it = kwargs.find(kStrAlgorithm);
  if (it != kwargs.end()) {
    const auto& it2 = kStr2AlgoType.find(it->second);
    if (it2 != kStr2AlgoType.end()) {
      random_option.set_algorithm(it2->second);
    }
  }

  it = kwargs.find(kStrMu);
  if (it != kwargs.end()) {
    random_option.set_mu(atof(it->second.c_str()));
  }

  it = kwargs.find(kStrSigma);
  if (it != kwargs.end()) {
    random_option.set_sigma(atof(it->second.c_str()));
  }

  it = kwargs.find(kStrAssignedValue);
  if (it != kwargs.end()) {
    random_option.set_assigned_value(atof(it->second.c_str()));
  }

  it = kwargs.find(kStrSeed);
  if (it != kwargs.end()) {
    random_option.set_seed(atof(it->second.c_str()));
  }

  return random_option;
}

template <typename DType>
Random<DType>::Random() {
  buffer_.resize(kRandomBufferSize);
  buffer_remaining_ = 0;
}

template <typename DType>
Random<DType>::Random(const RandomOption& random_option) : Random() {
  SetRandomOption(random_option);
}

template <typename DType>
void Random<DType>::SetRandomOption(const RandomOption& random_option) {
  random_option_ = random_option;
  rnd_engine_.seed(random_option_.seed());
}

template <typename DType>
void Random<DType>::Gen(DType* dptr, size_t size) {
  while (size > 0) {
    if (buffer_remaining_ == 0) {
      GenBuffer();
    }
    auto copy_size = std::min(buffer_remaining_, size);
    memcpy(dptr, buffer_.data() + kRandomBufferSize - buffer_remaining_,
           sizeof(DType) * copy_size);
    size -= copy_size;
    dptr += copy_size;
    buffer_remaining_ -= copy_size;
  }
}

template <typename DType>
void Random<DType>::GenBuffer() {
  switch (random_option_.algorithm()) {
    case kAssign:
      GenAssignBuffer();
      break;
    case kUniform:
      GenUniformBuffer();
      break;
    case kGaussian:
      GenGaussianBuffer();
      break;
    default:
      Log::Fatal("Unkown random algorithm: %u", random_option_.algorithm());
  }
  buffer_remaining_ = kRandomBufferSize;
}

template <typename DType>
void Random<DType>::GenAssignBuffer() {
  for (auto i = buffer_remaining_; i < kRandomBufferSize; ++i) {
    buffer_[i] = random_option_.assigned_value();
  }
}

template <typename DType>
void Random<DType>::GenUniformBuffer() {
  std::uniform_real_distribution<DType> dist(random_option_.min(), random_option_.max());
  for (auto i = buffer_remaining_; i < kRandomBufferSize; ++i) {
    buffer_[i] = static_cast<DType>(dist(rnd_engine_));
  }
}

template <typename DType>
void Random<DType>::GenGaussianBuffer() {
  std::normal_distribution<DType> dist(random_option_.mu(), random_option_.sigma());
  for (auto i = buffer_remaining_; i < kRandomBufferSize; ++i) {
    buffer_[i] = static_cast<DType>(dist(rnd_engine_));
  }
}

template class Random<float>;

}  // namespace hpps
