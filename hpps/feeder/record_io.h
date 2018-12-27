/*!
 * \file record_io.h
 * \brief record io for 
 */
#pragma once

#include <unordered_map>

#include "hpps/feeder/tensor.h"
#include "hpps/common/io/io.h"

namespace hpps {

class RecordIO {
 public:
  explicit RecordIO(const std::string& uri, FileOpenMode mode);
  virtual ~RecordIO();
  
  // Write header of sample record.
  void WriteHeader(const std::unordered_map<std::string, tensor_data_type_t>& tensor_meta);
  // Reader header of sample record
  void ReadHeader();
  // Write sample
  void WriteSample(const std::unordered_map<std::string, Tensor*>& sample);
  // Read sample and Return as array
  std::vector<Tensor*> ReadSampleAsArray();
  // Read sample and Return as map 
  std::unordered_map<std::string, Tensor*> ReadSampleAsMap();
  // Finalize
  void WriteFinalize();

  // Return vars
  sample_count_t sample_count() const { return sample_count_; }
  const std::vector<std::string>& names() const { return names_; }
  const std::vector<tensor_data_type_t>& types() const { return types_; }
  int version() const { return version_; }

 protected:
  Stream* stream_;
  sample_count_t sample_count_;
  std::vector<std::string> names_;
  std::vector<tensor_data_type_t> types_;
  int version_;
};

}  // namespace hpps
