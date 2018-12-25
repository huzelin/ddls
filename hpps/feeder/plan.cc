/*!
 * \file plan.cc
 * \brief The feeder plan
 */
#include "hpps/feeder/plan.h"

// [sample_count(8-byte)]
// [tensor_count(4-byte)]
// [tensor_name(32-byte)]
// [tensor_type(1-byte)]
// ....
// ....
// ....
// ....
// [tensor_count(2-byte)]
// [tensor_shape_count(1-byte)]
// [tensor_shape(4-byte * tensor_shape_count)]
// [tensor(sizeof(tensor_shape_type) * size)]
// ....

namespace hpps {

std::shared_ptr<Plan> PlanMaker::Make() {
  std::shared_ptr<Plan> plan(new Plan());
  plan->batch_size = batch_size_;

  for (const auto& u : uri_) {
    auto stream = StreamFactory::GetStream(u, FileOpenMode::BinaryRead);
    CHECK(stream->Good());
    
    // sample count
    feeder_sample_count_t sample_count;
    CHECK(sizeof(sample_count) == stream->Read(&sample_count, sizeof(sample_count)));
    plan->count += sample_count;
    
    // tensor count
    feeder_tensor_count_t tensor_count;
    CHECK(sizeof(tensor_count) == stream->Read(&tensor_count, sizeof(tensor_count)));
    
    for (auto i = 0; i < tensor_count; ++i) {
      char tensor_name[kTensorNameLen];
      CHECK(kTensorNameLen == stream->Read(tensor_name, kTensorNameLen));
      plan->names.push_back(tensor_name);
      
      feeder_tensor_type_t tensor_type;
      CHECK(sizeof(tensor_type) == stream->Read(&tensor_type, sizeof(tensor_type)));
      plan->types.push_back(tensor_type);
    }

    Plan::Block block = { stream, sample_count, plan.get() };
    plan->block.push_back(block);
  }
  return plan;
}

}  // namespace hpps
