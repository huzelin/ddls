/*
 * \file random_test.cc
 * \brief The random test unit
 */
#include "gtest/gtest.h"

#include <thread>

#include "hpps/common/random.h"
#include "hpps/common/log.h"

namespace hpps {

TEST(Random, GenAssigned) {
  RandomOption ro;
  ro.set_algorithm(kAssign);
  ro.set_assigned_value(2.0);
  Random<float> random(ro);
  float data[100];
  random.Gen(data, 100);
  for (auto i = 0; i < 100; ++i) {
    EXPECT_FLOAT_EQ(2.0, data[i]);
  }
}

TEST(Random, GenUniform) {
  RandomOption ro;
  ro.set_algorithm(kUniform);
  Random<float> random(ro);
  float data[100];
  random.Gen(data, 100);
  for (auto i = 0; i < 100; ++i) {
    EXPECT_TRUE(data[i] >= 0 && data[i] <= 1.0);
  }
}

TEST(Random, GenGaussian) {
  RandomOption ro;
  ro.set_algorithm(kGaussian);
  ro.set_mu(0);
  ro.set_sigma(1.0);
  Random<float> random(ro);
  float data[100];
  random.Gen(data, 100);
  float sum = 0;
  for (auto i = 0; i < 100; ++i) {
    sum += data[i];
  }
  float avg = sum / 100;
  EXPECT_TRUE(avg >= -1.0 && avg <= 1.0);
  Log::Info("Avg=%f", avg);
}

}  // namespace hpps
