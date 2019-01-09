/*
 * \file kvstore_test.cc
 * \brief The kvstore test unit
 */
#include "gtest/gtest.h"

#include <thread>

#include "hpps/kvstore/kvstore.h"

namespace hpps {

TEST(TestKVStore, GetOffset) {
  bool new_data;
  KVStore<uint64_t, float> kvstore(10000, 10);
  int64_t offset;
  size_t node_index;
  bool immutable = false;
  kvstore.Get(20, &node_index, &offset, immutable, new_data);
  EXPECT_EQ(10, offset);
  EXPECT_EQ(20, node_index);

  kvstore.Get(20, &node_index, &offset, immutable, new_data);
  EXPECT_EQ(10, offset);
  EXPECT_EQ(20, node_index);

  kvstore.Get(21, &node_index, &offset, immutable, new_data);
  EXPECT_EQ(20, offset);
  EXPECT_EQ(21, node_index);

  kvstore.Get(10020, &node_index, &offset, immutable, new_data);
  EXPECT_EQ(30, offset);
  EXPECT_EQ(10020, node_index);

  // Set
  std::vector<float> zeros(10, 0);
  kvstore.Set(0, zeros.data(), true);
  immutable = false;
  auto val = kvstore.Get(0, immutable, new_data);
  EXPECT_TRUE(immutable);

  for (auto i = 0; i < 10; ++i) {
    EXPECT_FLOAT_EQ(val[i], 0.0);
  }
}

}  // namespace hpps
