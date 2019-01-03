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
  kvstore.Get(20, &offset, false, &new_data);
  EXPECT_EQ(0, offset);

  kvstore.Get(20, &offset, false, &new_data);
  EXPECT_EQ(0, offset);

  kvstore.Get(21, &offset, false, &new_data);
  EXPECT_EQ(10, offset);

  kvstore.Get(10020, &offset, false, &new_data);
  EXPECT_EQ(20, offset);
}

TEST(TestKVStore, GetOffsetVectorization) {
  KVStore<uint64_t, float> kvstore(10000, 10);
  std::vector<uint64_t> keys;
  keys.push_back(20);
  keys.push_back(21);
  keys.push_back(10020);

  std::vector<int64_t> offsets(keys.size());
  bool new_data;
  kvstore.Get(keys.data(), keys.size(), const_cast<int64_t*>(offsets.data()), false, &new_data);

  EXPECT_EQ(0, offsets[0]);
  EXPECT_EQ(10, offsets[1]);
  EXPECT_EQ(20, offsets[2]);

  auto node = kvstore.node();
  EXPECT_EQ(20, node[20].key);
  EXPECT_EQ(0, node[20].offset);

  EXPECT_EQ(21, node[21].key);
  EXPECT_EQ(10, node[21].offset);

  EXPECT_EQ(10020, node[10020].key);
  EXPECT_EQ(20, node[10020].offset);
}

TEST(TestKVStore, Set) {

}

}  // namespace hpps
