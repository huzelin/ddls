/*
 * \file bench_feeder.cc
 * \brief The bench feeder
 */
#include <thread>

#include "hpps/feeder/feeder.h"
#include "hpps/feeder/plan.h"

namespace hpps {

void Bench() {
  PlanMaker plan_maker;
  plan_maker.set_uri({ "/tmp/sample1" });
  plan_maker.set_batch_size(8192);
  plan_maker.set_epoch(1);
  auto plan = plan_maker.Make();

  auto batch_iterator = Feeder::Get()->Schedule(plan);
  Feeder::Get()->Start(1);

  for (auto i = 0; i < 10; ++i) {
    std::unique_ptr<Batch> batch;
    batch_iterator->Pop(batch);
  }
  Feeder::Get()->Stop();
  LOG_INFO("Finalization");
}

}  // namespace hpps

int main(int argc, char** argv) {
  hpps::Bench();
  return 0;
}
