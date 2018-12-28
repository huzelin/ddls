/*!
 * \file record_io.cc
 * \brief record io implementation 
 */
#include "hpps/feeder/record_io.h"

#include "hpps/common/log.h"
#include "hpps/common/version.h"

namespace hpps {

RecordIO::RecordIO(const std::string& uri, FileOpenMode mode) {
  stream_ = StreamFactory::GetStream(URI(uri), mode);
  sample_count_ = 0;
}

RecordIO::~RecordIO() {
  delete stream_;
  stream_ = nullptr;
}
  
void RecordIO::WriteHeader(
    const std::unordered_map<std::string, tensor_data_type_t>& tensor_meta) {
  for (const auto& item : tensor_meta) {
    names_.push_back(item.first);
    types_.push_back(item.second);
  }
  version_ = HPPS_VERSION;
  stream_->Write(&version_, sizeof(version_));
  stream_->Write(&sample_count_, sizeof(sample_count_));
  tensor_count_t tensor_count = names_.size();
  stream_->Write(&tensor_count,sizeof(tensor_count));

  char buf[kTensorNameLen];
  tensor_data_type_t data_type;
  for (auto i = 0; i < names_.size(); ++i) {
    snprintf(buf, kTensorNameLen, names_[i].c_str());
    data_type = types_[i];
    stream_->Write(buf, kTensorNameLen);
    stream_->Write(&data_type, sizeof(data_type));
  }
}

void RecordIO::ReadHeader() {
  stream_->Read(&version_, sizeof(version_));
  stream_->Read(&sample_count_, sizeof(sample_count_));
  tensor_count_t tensor_count;
  stream_->Read(&tensor_count, sizeof(tensor_count));
  Log::Debug("version_=%u", version_);
  Log::Debug("sample_count_=%u", sample_count_);

  char buf[kTensorNameLen];
  tensor_data_type_t data_type;
  for (auto i = 0; i < tensor_count; ++i) {
    stream_->Read(buf, kTensorNameLen);
    names_.push_back(buf);
    stream_->Read(&data_type, sizeof(data_type));
    types_.push_back(data_type);
  }
}

void RecordIO::WriteSample(const std::unordered_map<std::string, Tensor*>& sample) {
  tensor_count_t count = names_.size();
  stream_->Write(&count, sizeof(count));

  for (const auto& name : names_) {
    const auto iter = sample.find(name);
    CHECK(iter != sample.end());
    auto tensor = iter->second;

    const auto& shape = tensor->shape();
    tensor_dim_count_t dim_count = shape.size();
    stream_->Write(&dim_count, sizeof(dim_count));

    for (const auto& dim : shape) {
      stream_->Write(&dim, sizeof(dim));
    }

    auto blob = tensor->mutable_blob();
    stream_->Write(blob->data(), blob->size());
  }
  ++sample_count_;
}

std::vector<Tensor*> RecordIO::ReadSampleAsArray() {
  std::vector<Tensor*> tensors;

  tensor_count_t count;
  stream_->Read(&count, sizeof(count));
  CHECK(count == names_.size());

  for (auto i = 0; i < count; ++i) {
    tensor_dim_count_t dim_count;
    stream_->Read(&dim_count, sizeof(dim_count));

    std::vector<tensor_dim_t> shape;
    for (auto k = 0; k < dim_count; ++k) {
      tensor_dim_t dim;
      stream_->Read(&dim, sizeof(dim));
      shape.push_back(dim);
    }

    auto tensor = new Tensor(shape, types_[i]);
    auto blob = tensor->mutable_blob();
    stream_->Read(blob->data(), blob->size());

    tensors.push_back(tensor);
  }
  return tensors;
}

std::unordered_map<std::string, Tensor*> RecordIO::ReadSampleAsMap() {
  auto array = ReadSampleAsArray();
  std::unordered_map<std::string, Tensor*> map;
  for (auto i = 0; i < array.size(); ++i) {
    map[names_[i]] = array[i];
  }
  return map;
}

void RecordIO::WriteFinalize() {
  stream_->Seek(sizeof(version_));
  stream_->Write(&sample_count_, sizeof(sample_count_));
}

}  // namespace hpps
