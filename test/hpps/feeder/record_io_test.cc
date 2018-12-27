/*
 * \file record_io_test.cc
 * \brief The record io test unit
 */
#include "gtest/gtest.h"

#include <thread>

#include "hpps/feeder/record_io.h"
#include "hpps/common/version.h"

namespace hpps {

TEST(RecordIO, Write) {
  RecordIO record_io("./record_io.dat", FileOpenMode::BinaryWrite);
  std::unordered_map<std::string, tensor_data_type_t> meta;
  meta["user_id"] = kFloat32;
  meta["item_id"] = kFloat32;
  record_io.WriteHeader(meta);
  record_io.WriteFinalize();
}

TEST(RecordIO, Read) {
  RecordIO record_io("./record_io.dat", FileOpenMode::BinaryRead);
  record_io.ReadHeader();
  EXPECT_EQ(0, record_io.sample_count());
  EXPECT_EQ((HPPS_VERSION), record_io.version());
  EXPECT_EQ(2, record_io.names().size());
  EXPECT_EQ(2, record_io.types().size());
  EXPECT_EQ(kFloat32, record_io.types()[0]);
  EXPECT_EQ(kFloat32, record_io.types()[1]);
}

TEST(RecordIO, WriteSample) {
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

TEST(RecordIO, ReadSample) {
  RecordIO record_io("./record_io.dat", FileOpenMode::BinaryRead);
  record_io.ReadHeader();
  EXPECT_EQ(2, record_io.sample_count());
  EXPECT_EQ((HPPS_VERSION), record_io.version());
  EXPECT_EQ(2, record_io.names().size());
  EXPECT_EQ(2, record_io.types().size());
  EXPECT_EQ(kFloat32, record_io.types()[0]);
  EXPECT_EQ(kFloat32, record_io.types()[1]);

  auto array = record_io.ReadSampleAsArray();
  EXPECT_EQ(2, array.size());
  auto map = record_io.ReadSampleAsMap();
  EXPECT_EQ(2, map.size());

  auto comm_tensor = map["user_id"];
  EXPECT_EQ(2, comm_tensor->shape().size());
  EXPECT_FLOAT_EQ(0.3, comm_tensor->mutable_blob()->As<float>(0));
  auto ncomm_tensor = map["item_id"];
  EXPECT_EQ(2, ncomm_tensor->shape().size());
  EXPECT_FLOAT_EQ(0.4, ncomm_tensor->mutable_blob()->As<float>(0));
}

}  // namespace hpps

