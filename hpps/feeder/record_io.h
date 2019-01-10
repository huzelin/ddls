/*!
 * \file record_io.h
 * \brief record io for 
 */
#pragma once

#include <unordered_map>

#include "hpps/common/tensor.h"
#include "hpps/common/io/io.h"

namespace hpps {

class RecordIO {
 public:
  explicit RecordIO(const std::string& uri, FileOpenMode mode);
  virtual ~RecordIO();

  struct ItemConfig {
    ItemConfig() { }
    explicit ItemConfig(tensor_data_type_t type) : type(type) {
      level = 0;
      is_aux_number = 0;
    }
    tensor_data_type_t type; // tensor type
    int level; // level 0 is major table
    int8_t is_aux_number;  // is aux number
  };

  // Write header of sample record.
  void WriteHeader(const std::unordered_map<std::string, ItemConfig>& tensor_meta);
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
  const std::vector<ItemConfig>& configs() const { return configs_; }
  int version() const { return version_; }

 protected:
  Stream* stream_;
  sample_count_t sample_count_;
  std::vector<std::string> names_;
  std::vector<ItemConfig> configs_;
  int version_;
};

}  // namespace hpps
