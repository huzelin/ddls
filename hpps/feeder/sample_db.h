/*!
 * \file sample_db.h
 * \brief The sample database
 */
#pragma once

#include "hpps/common/io/io.h"

namespace hpps {

class SampleDb {
 public:
  explicit SampleDb(const std::vector<std::string>& uri);
  virtual ~SampleDb();

  // Set the epoch
  void set_epoch(int epoch) { epoch_ = epoch; } 
  int epoch() const { return epoch_; } 

 protected:
  std::vector<Stream*> stream_;
  int epoch_;
};

}  // namespace hpps
