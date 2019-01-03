/*
 * \file zoo_kv_test.cc
 * \brief The zoo kv test unit
 */
#include "gtest/gtest.h"

#include <thread>

#include "hpps/common/dashboard.h"
#include "hpps/frame/table_factory.h"
#include "hpps/frame/zoo.h"
#include "hpps/table/kv_table.h"

namespace hpps {

TEST(Zoo, Start) {
  auto zoo = Zoo::Get();
  int argc = 2;
  char* data[] = { const_cast<char*>("-machine_file=./utest_data/hostfile"),
                   const_cast<char*>("-updater_type=default"),
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
  KVTableOption<uint64_t, float> kv_table_option(10000, 16);
  kv_table_option.random_option.set_algorithm(kAssign);
  kv_table_option.random_option.set_assigned_value(2.0);

  auto table = table_factory::CreateTable(kv_table_option);

  if (table != nullptr) {
    for (int loop = 0; loop < 2; ++loop) {
      // Step1: get the parameters
      std::vector<uint64_t> keys;
      keys.push_back(12UL);
      table->Get(keys);

      // Step2: get the local parameters
      float* data = table->raw().Get(keys[0], true);
      EXPECT_TRUE(data != nullptr);
      for (auto i = 0; i < 16; ++i) {
        LOG_INFO("data[%d]=%f", i, data[i]);
      }

      // Step3: update grad
      std::vector<float> grads;
      for (auto i = 0; i < 16; ++i) {
        grads.push_back(i);
      }
      table->Add(keys, grads);
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
