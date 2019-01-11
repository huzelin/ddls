/*
 * \file feeder_test.cc
 * \brief The feeder test unit
 */
#include "gtest/gtest.h"

#include <thread>

#include "hpps/feeder/feeder.h"
#include "hpps/feeder/record_io.h"

namespace hpps {

void WriteSample() {
  RecordIO record_io("./record_io.dat", FileOpenMode::BinaryWrite);
  std::unordered_map<std::string, tensor_data_type_t> meta;
  meta["user_id"] = kFloat32;
  meta["item_id"] = kFloat32;
  record_io.WriteHeader(meta);

  auto comm_tensor = new Tensor({ 3, 3 }, kFloat32);
  auto ncomm_tensor = new Tensor({ 3, 2 }, kFloat32);
  std::unordered_map<std::string, Tensor*> sample;
  sample["user_id"] = comm_tensor;
  sample["item_id"] = ncomm_tensor;
  // the first sample
  for (auto i = 0; i < comm_tensor->size(); ++i) {
    comm_tensor->mutable_blob()->As<float>(i) = 0.1 + i;
    ncomm_tensor->mutable_blob()->As<float>(i) = 0.2 + i;
  }
  record_io.WriteSample(sample);
  // the second sample
  for (auto i = 0; i < comm_tensor->size(); ++i) {
    comm_tensor->mutable_blob()->As<float>(i) = 0.3 + i;
    ncomm_tensor->mutable_blob()->As<float>(i) = 0.4 + i;
  }
  record_io.WriteSample(sample);

  record_io.WriteFinalize();
}

TEST(Feeder, Feeder) {
  WriteSample();

  PlanMaker plan_maker;
  plan_maker.set_uri({ "./record_io.dat" });
  plan_maker.set_batch_size(2);

  auto plan = plan_maker.Make();

  auto iterator = Feeder::Get()->Schedule(plan);

  Feeder::Get()->Start(1);
  
  std::unique_ptr<Batch> batch;
  iterator->Pop(batch);
  batch->Get("user_id")->PrintDebug();
  batch->Get("item_id")->PrintDebug();

  iterator->Pop(batch);
  batch->Get("user_id")->PrintDebug();
  batch->Get("item_id")->PrintDebug();

  Feeder::Get()->Stop();
}

}  // namespace hpps

