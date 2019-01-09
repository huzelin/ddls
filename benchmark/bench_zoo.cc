/*
 * \file zoo_test.cc
 * \brief The zoo test unit
 */
#include <thread>

#include "hpps/common/dashboard.h"
#include "hpps/common/timer.h"
#include "hpps/frame/table_factory.h"
#include "hpps/frame/zoo.h"
#include "hpps/table/array_table.h"

namespace hpps {

void Bench() {
  auto zoo = Zoo::Get();
  int argc = 3;
  char* data[] = { const_cast<char*>("-machine_file=./utest_data/hostfile"),
                   const_cast<char*>("-net_type=mpi"),
                   const_cast<char*>("-sync=false") };
  zoo->Start(&argc, data);
  
  const int kSize = 1024 * 1024;  // 1 MB model size
  
  ArrayTableOption<float> array_table_option(kSize, "", "sync");
  array_table_option.random_option.set_algorithm(kAssign);
  array_table_option.random_option.set_assigned_value(2.0);

  auto table = table_factory::CreateTable(array_table_option);

  zoo->Barrier();
  Timer timer;
  timer.Start();

  if (table != nullptr) {
    float* data = new float[kSize];
    float* delta = new float[kSize];

    for (int k = 0; k < 500; ++k) {
      // Step1: Get
      table->Get(data, kSize);

      // Step2: Add
      delta[0] = 1.2;
      table->Add(delta, kSize);

      if (zoo->rank() == 0) {
        Log::Info("iter=%d", k);
      }
    }
    delete [] data;
    delete [] delta;
  }
  zoo->Barrier();

  if (zoo->rank() == 0) {
    Dashboard::Display();
    Log::Info("Total time=%f", timer.Elapse());
  }

  zoo->Stop(true);
}

}  // namespace hpps

int main(int argc, char** argv) {
  hpps::Bench();
  return 0;
}
