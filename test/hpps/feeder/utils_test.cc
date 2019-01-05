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
  delete num;
}

TEST(Utils, Id2UniqId) {
  Tensor* id = new Tensor({ 4 }, kUInt64);
  id->mutable_blob()->As<uint64_t>(0) = 12;
  id->mutable_blob()->As<uint64_t>(1) = 0;
  id->mutable_blob()->As<uint64_t>(2) = 13;
  id->mutable_blob()->As<uint64_t>(3) = 12;
  
  auto result = Id2UniqId(id);
  auto uniq_id = result[0];
  auto local_id = result[1];

  EXPECT_EQ(3, uniq_id->mutable_blob()->size<uint64_t>());
  EXPECT_EQ(12, uniq_id->mutable_blob()->As<uint64_t>(0));
  EXPECT_EQ(0, uniq_id->mutable_blob()->As<uint64_t>(1));
  EXPECT_EQ(13, uniq_id->mutable_blob()->As<uint64_t>(2));

  EXPECT_EQ(4, local_id->mutable_blob()->size<uint32_t>());
  EXPECT_EQ(0, local_id->mutable_blob()->As<uint32_t>(0));
  EXPECT_EQ(1, local_id->mutable_blob()->As<uint32_t>(1));
  EXPECT_EQ(2, local_id->mutable_blob()->As<uint32_t>(2));
  EXPECT_EQ(0, local_id->mutable_blob()->As<uint32_t>(3));

  delete id;
  delete uniq_id;
  delete local_id;
}

}  // namespace hpps

