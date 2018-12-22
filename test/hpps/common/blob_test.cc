/*
 * \file blob_test.cc
 * \brief The blob test unit
 */
#include "gtest/gtest.h"

#include <thread>

#include "hpps/common/blob.h"

namespace hpps {

TEST(Blob, Blob) {
  Blob blob("Hello World", strlen("Hello World"));
  EXPECT_EQ(blob[0], 'H');
  EXPECT_EQ(blob[2], 'l');

  Blob new_blob(blob);
  EXPECT_EQ(new_blob[0], 'H');
  EXPECT_EQ(new_blob[2], 'l');
}

}  // namespace hpps
