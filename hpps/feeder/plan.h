/*!
 * \file plan.h
 * \brief The feeder plan
 */
#pragma once

#include "hpps/common/io/io.h"
#include "hpps/feeder/common.h"

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
  // Return the tensor count
  size_t tensor_count() const { return names.size(); }

  std::vector<Block> block;
  // The total sample count
  size_t count;
  // The batch size you want
  int batch_size;
  // The tensor name array
  std::vector<std::string> names;
  // The tensor type array
  std::vector<uint8_t> types;
};

class PlanMaker {
 public:
  // Make the Batch assemble plan.
  std::shared_ptr<Plan> Make();
  
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
