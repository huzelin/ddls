/*
 * \file waiter_test.cc
 * \brief The waiter test unit
 */
#include "gtest/gtest.h"

#include <thread>

#include "hpps/common/waiter.h"

namespace hpps {

void JobRunner(Waiter* waiter) {
  waiter->Notify();
  waiter->Notify();
}

TEST(Waiter, TestAll) {
  Waiter waiter(2);
  std::thread thread1(JobRunner, &waiter);
  waiter.Wait();
  EXPECT_TRUE(true);
  thread1.join();

  waiter.Reset(2);
  std::thread thread2(JobRunner, &waiter);
  waiter.Wait();
  EXPECT_TRUE(true);
  thread2.join();
}

}  // namespace hpps

