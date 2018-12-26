/*!
 * \file plan.h
 * \brief The feeder plan
 */
#pragma once

#include "hpps/common/io/io.h"
#include "hpps/feeder/types.h"

#include <memory>

namespace hpps {

struct Plan {
  Plan() : count(0) { }
  // The block [start, end) sample segment
  // block can be read parallelly.
  struct Block {
    Stream* stream;
    size_t count;
    Plan* plan;
  };

  std::vector<Block> block;
  // The total sample count
  size_t count;
  // The batch size you want
  int batch_size;
  // The tensor name array
  std::vector<std::string> tensor_names;
  // The tensor type array
  std::vector<tensor_data_type_t> tensor_data_types;
};

class PlanMaker {
 public:
  PlanMaker() : epoch_(2), batch_size_(128) { }
  // Make the Batch assemble plan.
  Plan* Make();
  
  void set_uri(const std::vector<URI>& uri) { uri_ = uri; }
  const std::vector<URI>& uri() const { return uri_; }

  void set_epoch(int epoch) { epoch_ = epoch; }
  int epoch() const { return epoch_; }

  void set_batch_size(int batch_size) { batch_size_ = batch_size; }
  int batch_size() const { return batch_size_; }

 protected:
  // The sample URI
  std::vector<URI> uri_;
  // The epoch number
  int epoch_;
  // The batch size
  int batch_size_;
};

}  // namespace hpps
