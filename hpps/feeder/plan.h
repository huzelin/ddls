/*!
 * \file plan.h
 * \brief The feeder plan
 */
#pragma once

#include "hpps/common/io/io.h"
#include "hpps/common/types.h"
#include "hpps/feeder/record_io.h"

#include <memory>

namespace hpps {

struct Plan {
  Plan() : count(0) { }
  // SampleRecord means a sample file.
  struct SampleRecord {
    std::shared_ptr<RecordIO> record_io;
    Plan* plan;
  };

  std::vector<SampleRecord> sample_record;
  // The total sample count
  size_t count;
  // The batch size you want
  int batch_size;
};

class PlanMaker {
 public:
  PlanMaker() : epoch_(1), batch_size_(128) { }
  // Make the Batch assemble plan.
  Plan* Make();
  
  void set_uri(const std::vector<std::string>& uri) { uri_ = uri; }
  const std::vector<std::string>& uri() const { return uri_; }

  void set_epoch(int epoch) { epoch_ = epoch; }
  int epoch() const { return epoch_; }

  void set_batch_size(int batch_size) { batch_size_ = batch_size; }
  int batch_size() const { return batch_size_; }

 protected:
  // The sample URI
  std::vector<std::string> uri_;
  // The epoch number
  int epoch_;
  // The batch size
  int batch_size_;
};

}  // namespace hpps
