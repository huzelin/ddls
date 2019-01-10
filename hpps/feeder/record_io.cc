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

void RecordIO::WriteHeader(const std::unordered_map<std::string, ItemConfig>& tensor_meta) {
  for (const auto& item : tensor_meta) {
    names_.push_back(item.first);
    configs_.push_back(item.second);
  }
  version_ = HPPS_VERSION;
  stream_->Write(&version_, sizeof(version_));
  stream_->Write(&sample_count_, sizeof(sample_count_));
  tensor_count_t tensor_count = names_.size();
  stream_->Write(&tensor_count,sizeof(tensor_count));

  char buf[kTensorNameLen];
  for (auto i = 0; i < names_.size(); ++i) {
    snprintf(buf, kTensorNameLen, names_[i].c_str());
    stream_->Write(buf, kTensorNameLen);
    stream_->Write(&(configs_[i].type), sizeof(configs_[i].type));
    stream_->Write(&(configs_[i].level), sizeof(configs_[i].level));
    stream_->Write(&(configs_[i].is_aux_number), sizeof(configs_[i].is_aux_number));
  }
}
  
void RecordIO::WriteHeader(
    const std::unordered_map<std::string, tensor_data_type_t>& tensor_meta) {
  std::unordered_map<std::string, ItemConfig> map;
  for (const auto& item : tensor_meta) {
    map[item.first] = ItemConfig(item.second);
  }
  WriteHeader(map);
}

void RecordIO::ReadHeader() {
  stream_->Read(&version_, sizeof(version_));
  stream_->Read(&sample_count_, sizeof(sample_count_));
  tensor_count_t tensor_count;
  stream_->Read(&tensor_count, sizeof(tensor_count));
  LOG_INFO("version_=%u", version_);
  LOG_INFO("sample_count_=%u", sample_count_);
  LOG_INFO("tensor_count=%u", tensor_count);

  names_.clear();
  configs_.clear();

  char buf[kTensorNameLen];
  for (auto i = 0; i < tensor_count; ++i) {
    stream_->Read(buf, kTensorNameLen);
    names_.push_back(buf);
    ItemConfig config;
    stream_->Read(&(config.type), sizeof(config.type));
    stream_->Read(&(config.level), sizeof(config.level));
    stream_->Read(&(config.is_aux_number), sizeof(config.is_aux_number));
    configs_.push_back(config);
    LOG_DEBUG("name=%s type=%d level=%d is_aux_number=%d",
              names_[i].c_str(),
              configs_[i].type,
              configs_[i].level,
              configs_[i].is_aux_number);
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
  LOG_DEBUG("names_.size()=%u", names_.size());
  std::vector<Tensor*> tensors;

  tensor_count_t count;
  stream_->Read(&count, sizeof(count));
  LOG_DEBUG("count=%u names_.size()=%u", count, names_.size());
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

    auto tensor = new Tensor(shape, configs_[i].type);
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

  LOG_INFO("sample_count=%u tensor_count=%u", sample_count_, names_.size());
  for (auto i = 0; i < names_.size(); ++i) {
    LOG_INFO("name=%s dtype=%d", names_[i].c_str(), configs_[i].type);
  }
}

}  // namespace hpps
