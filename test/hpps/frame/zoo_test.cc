/*
 * \file zoo_test.cc
 * \brief The zoo test unit
 */
#include "gtest/gtest.h"

#include <thread>

#include "hpps/common/dashboard.h"
#include "hpps/frame/table_factory.h"
#include "hpps/frame/zoo.h"
#include "hpps/table/array_table.h"

namespace hpps {

TEST(Zoo, Start) {
  auto zoo = Zoo::Get();
  int argc = 1;
  char* data[] = { const_cast<char*>("-machine_file=./utest_data/hostfile"),
                   const_cast<char*>("-net_type=mpi") };
  zoo->Start(&argc, data);
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
  ArrayTableOption<float> array_table_option(100);
  array_table_option.random_option.set_algorithm(kAssign);
  array_table_option.random_option.set_assigned_value(2.0);

  auto table = table_factory::CreateTable(array_table_option);

  if (table != nullptr) {
#define SIZE 100
    // Step1: Get
    float data[SIZE];
    table->Get(data, SIZE);
    for (auto i = 0; i < SIZE; ++i) {
      EXPECT_FLOAT_EQ(data[i], 2.0);
    }
    // Step2: Add
    float delta[SIZE] = { 0 };
    delta[0] = 1.2;
    table->Add(delta, SIZE);
    // Step3: Get
    table->Get(data, SIZE);
    for (auto i = 0; i < SIZE; ++i) {
      if (i == 0) {
        EXPECT_FLOAT_EQ(data[i], 3.2);
      } else {
        EXPECT_FLOAT_EQ(data[i], 2.0);
      }
    }
  }
  Dashboard::Display();
}

TEST(Zoo, Stop) {
  auto zoo = Zoo::Get();
  zoo->Stop(true);
  table_factory::FreeServerTables();
}

}  // namespace hpps
