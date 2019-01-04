/*
 * \file utils_test.cc
 * \brief The utils test unit
 */
#include "gtest/gtest.h"

#include <thread>

#include "hpps/feeder/utils.h"

namespace hpps {

TEST(Utils, Num2Indices) {
  Tensor* num = new Tensor({ 4 }, kUInt32);
  num->mutable_blob()->As<uint32_t>(0) = 2;
  num->mutable_blob()->As<uint32_t>(1) = 3;
  num->mutable_blob()->As<uint32_t>(2) = 2;
  num->mutable_blob()->As<uint32_t>(3) = 2;

  auto indices = Num2Indices(num);
  for (auto i = 0; i < 2; ++i) {
    EXPECT_EQ(0, indices->mutable_blob()->As<uint32_t>(i));
  }
  for (auto i = 2; i< 5; ++i) {
    EXPECT_EQ(1, indices->mutable_blob()->As<uint32_t>(i));
  }
  for (auto i = 5; i < 7; ++i) {
    EXPECT_EQ(2, indices->mutable_blob()->As<uint32_t>(i));
  }
  for (auto i = 7; i < 9; ++i) {
    EXPECT_EQ(3, indices->mutable_blob()->As<uint32_t>(i));
  }
}

TEST(Utils, Id2UniqId) {

}

}  // namespace hpps

