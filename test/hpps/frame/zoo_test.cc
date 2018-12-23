/*
 * \file zoo_test.cc
 * \brief The zoo test unit
 */
#include "gtest/gtest.h"

#include <thread>

#include "hpps/common/dashboard.h"
#include "hpps/frame/zoo.h"
#include "hpps/table/array_table.h"

namespace hpps {

TEST(Zoo, Start) {
  auto zoo = Zoo::Get();
  int argc = 0;
  zoo->Start(&argc, nullptr);
}

TEST(Zoo, Barrier) {
  auto zoo = Zoo::Get();
  zoo->Barrier();
}

TEST(Zoo, worker_rank) {
  auto zoo = Zoo::Get();
  EXPECT_EQ(zoo->worker_rank(), 0);
}

TEST(Zoo, server_rank) {
  auto zoo = Zoo::Get();
  EXPECT_EQ(zoo->server_rank(), 0);
}

TEST(Zoo, RegisterTable) {
  auto zoo = Zoo::Get();
  auto array_server = new ArrayServer<float>(100);
  auto table_id = zoo->RegisterTable(array_server);
  EXPECT_EQ(1, table_id);

  auto array_worker = new ArrayWorker<float>(100);
  table_id = zoo->RegisterTable(array_worker);
  EXPECT_EQ(1, table_id);

  float data[100];
  array_worker->Get(data, 100);
  Dashboard::Display();
}

TEST(Zoo, Stop) {
  auto zoo = Zoo::Get();
  zoo->Stop(true);
}

}  // namespace hpps
