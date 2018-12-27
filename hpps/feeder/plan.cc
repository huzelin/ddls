/*!
 * \file plan.cc
 * \brief The feeder plan
 */
#include "hpps/feeder/plan.h"

// [sample_count(8-byte)]
// [tensor_count(4-byte)]
// [tensor_name(32-byte)]
// [tensor_data_type(1-byte)]
// ....
// ....
// ....
// ....
// [tensor_count(2-byte)]
// [tensor_dim_count(1-byte)]
// [tensor_dim (4-byte * tensor_dim_count)]
// [tensor(sizeof(tensor_dim_type) * size)]
// ....

namespace hpps {

Plan* PlanMaker::Make() {
  Plan* plan = new Plan();
  plan->batch_size = batch_size_;
  plan->count = 0;
  CHECK(batch_size_ > 0);

  for (auto i = 0; i < epoch_; ++i) {
    for (const auto& u : uri_) {
      auto record_io = new RecordIO(u, FileOpenMode::BinaryRead);
      record_io->ReadHeader();
      
      plan->count += record_io->sample_count();
      
      Plan::SampleRecord sample_record;
      sample_record.plan = plan;
      sample_record.record_io.reset(record_io);
      
      plan->sample_record.push_back(sample_record);
    }
  }
  return plan;
}

}  // namespace hpps
