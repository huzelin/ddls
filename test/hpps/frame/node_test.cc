/*
 * \file node_test.cc
 * \brief The node test unit
 */
#include "gtest/gtest.h"

#include <thread>

#include "hpps/frame/node.h"

namespace hpps {

TEST(Node, TestAll) {
  int role = 3;
  EXPECT_TRUE(node::is_worker(role));
  EXPECT_TRUE(node::is_server(role));

  role = 1;
  EXPECT_TRUE(node::is_worker(role));
  EXPECT_FALSE(node::is_server(role));

  role = 2;
  EXPECT_FALSE(node::is_worker(role));
  EXPECT_TRUE(node::is_server(role));
}

}  // namespace hpps
