/*
 * \file message_test.cc
 * \brief The message test unit
 */
#include "gtest/gtest.h"

#include <thread>

#include "hpps/common/message.h"
#include "hpps/common/log.h"

namespace hpps {

TEST(Message, type) {
  Message message;
  message.set_type(Request_Get);
  EXPECT_EQ(message.type(), Request_Get);
}

TEST(Message, src) {
  Message message;
  message.set_src(1);
  EXPECT_EQ(message.src(), 1);
}

TEST(Message, dst) {
  Message message;
  message.set_dst(2);
  EXPECT_EQ(message.dst(), 2);
}

TEST(Message, table_id) {
  Message message;
  message.set_table_id(3);
  EXPECT_EQ(message.table_id(), 3);
}

TEST(Message, msg_id) {
  Message message;
  message.set_msg_id(4);
  EXPECT_EQ(message.msg_id(), 4);
}

}  // namespace hpps

