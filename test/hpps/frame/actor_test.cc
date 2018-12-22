/*
 * \file actor_test.cc
 * \brief The actor test unit
 */
#include "gtest/gtest.h"

#include <thread>

#include "hpps/frame/actor.h"

namespace hpps {

TEST(Actor, TestAll) {
  Actor* actor = new Actor("worker");
  actor->Start();
  actor->Stop();
  delete actor;
}

}  // namespace hpps
