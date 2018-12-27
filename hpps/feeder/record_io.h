/*!
 * \file record_io.h
 * \brief record io for 
 */
#include "hpps/feeder/tensor.h"

namespace hpps {

class RecordIO {
 public:
  explicit RecordIO(const std::string& uri);
  
  // Write header of sample record.
  void WriteHeader(const std::unordered_map<std::string, tensor_data_type_t>& tensor_meta);
  // Write sample
  void WriteSample(const std::unordered_map<std::string, Tensor*>& sample);

 protected:
  std::shared_ptr<Stream> stream_;
  sample_count_t sample_count_;
  std::vector<std::string> names_;
  std::vector<tensor_data_type_t> types_;
};

}  // namespace hpps
