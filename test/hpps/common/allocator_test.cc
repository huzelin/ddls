/*
 * \file allocator_test.cc
 * \brief The allocator test unit
 */
#include "gtest/gtest.h"

#include <thread>

#include "hpps/common/allocator.h"
#include "hpps/common/log.h"

namespace hpps {

TEST(Allocator, Alloc) {
  auto allocator = Allocator::Get();
  EXPECT_TRUE(allocator != nullptr);

  auto data = allocator->Alloc(10);
  Log::Info("data=%p", data);
  
  allocator->Free(data);

  auto data2 = allocator->Alloc(10);
  EXPECT_EQ(data, data2);
}

TEST(Allocator, Alloc2) {
  auto allocator = Allocator::Get();
  EXPECT_TRUE(allocator != nullptr);

  auto data = allocator->Alloc(10);
  allocator->Refer(data);

  allocator->Free(data);

  auto data2 = allocator->Alloc(10);
  EXPECT_TRUE(data != data2);

  allocator->Free(data);
  data2 = allocator->Alloc(10);
  EXPECT_EQ(data, data2);
}

}  // namespace hpps
