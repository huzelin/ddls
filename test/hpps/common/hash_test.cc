/*
 * \file hash_test.cc
 * \brief The hash test unit
 */
#include "gtest/gtest.h"

#include <thread>

#include "hpps/common/hash.h"
#include "hpps/common/log.h"

namespace hpps {

TEST(Hash, MurmurHash64B) {
  std::string str = "hello world";
  auto hash = MurmurHash64B(str.data(), str.length());
  Log::Info("hash=%u", hash);
}

}  // namespace hpps
