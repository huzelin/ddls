/*
 * \file zoo_test.cc
 * \brief The zoo test unit
 */
#include "gtest/gtest.h"

#include <thread>

#include "hpps/frame/zoo.h"

namespace hpps {

TEST(Zoo, Start) {
  auto zoo = Zoo::Get();
  int argc = 0;
  zoo->Start(&argc, nullptr);
  zoo->Stop(true);
}

}  // namespace hpps
