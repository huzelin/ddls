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

  auto comm_tensor = new Tensor({ 2, 3 }, kFloat32);
  auto ncomm_tensor = new Tensor({ 3, 2 }, kFloat32);
  std::unordered_map<std::string, Tensor*> sample;
  sample["user_id"] = comm_tensor;
  sample["item_id"] = ncomm_tensor;
  // the first sample
  comm_tensor->mutable_blob()->As<float>(0) = 0.1;
  ncomm_tensor->mutable_blob()->As<float>(0) = 0.2;
  record_io.WriteSample(sample);
  // the second sample
  comm_tensor->mutable_blob()->As<float>(0) = 0.3;
  ncomm_tensor->mutable_blob()->As<float>(0) = 0.4;
  record_io.WriteSample(sample);

  record_io.WriteFinalize();
}

TEST(Feeder, Feeder) {
  WriteSample();

  PlanMaker plan_maker;
  plan_maker.set_uri({ "./record_io.dat" });
  plan_maker.set_batch_size(1);

  auto plan = plan_maker.Make();

  auto iterator = Feeder::Get()->Schedule(plan);

  Feeder::Get()->Start(10);
  
  std::shared_ptr<Batch> batch;
  iterator->Pop(batch);
  iterator->Pop(batch);
}

}  // namespace hpps

