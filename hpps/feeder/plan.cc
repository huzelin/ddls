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
  CHECK(batch_size_ > 0);

  for (const auto& u : uri_) {
    auto stream = StreamFactory::GetStream(u, FileOpenMode::BinaryRead);
    CHECK(stream->Good());
    
    // sample count
    sample_count_t sample_count;
    CHECK(sizeof(sample_count) == stream->Read(&sample_count, sizeof(sample_count)));
    plan->count += sample_count;
    
    // tensor count
    tensor_count_t tensor_count;
    CHECK(sizeof(tensor_count) == stream->Read(&tensor_count, sizeof(tensor_count)));
    
    for (auto i = 0; i < tensor_count; ++i) {
      char tensor_name[kTensorNameLen];
      CHECK(kTensorNameLen == stream->Read(tensor_name, kTensorNameLen));
      plan->tensor_names.push_back(tensor_name);
      
      tensor_data_type_t tensor_data_type;
      CHECK(sizeof(tensor_data_type) == stream->Read(&tensor_data_type, sizeof(tensor_data_type)));
      plan->tensor_data_types.push_back(tensor_data_type);
    }

    Plan::Block block = { stream, sample_count, plan };
    plan->block.push_back(block);
  }
  return plan;
}

}  // namespace hpps
