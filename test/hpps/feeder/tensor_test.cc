/*
 * \file tensor_test.cc
 * \brief The tensor test unit
 */
#include "gtest/gtest.h"

#include <thread>

#include "hpps/feeder/tensor.h"

namespace hpps {

TEST(Tensor, Tensor) {
  std::vector<tensor_dim_t> shp = { 2, 3 };
  tensor_data_type_t data_type = kFloat32;
  Tensor tensor(shp, data_type);

  auto shape = tensor.shape();
  EXPECT_EQ(2, shape.size());
  EXPECT_EQ(2, shape[0]);
  EXPECT_EQ(3, shape[1]);
  EXPECT_EQ(6, tensor.size());
  EXPECT_EQ(kFloat32, tensor.data_type());

  EXPECT_EQ(24, tensor.mutable_blob()->size());
}

}  // namespace hpps

