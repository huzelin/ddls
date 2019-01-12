/*!
 * \file record_io.cc
 * \brief record io implementation 
 */
#include "hpps/feeder/record_io.h"

#include "hpps/common/log.h"
#include "hpps/common/types.h"
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
  LOG_DEBUG("version_=%u", version_);
  LOG_DEBUG("sample_count_=%u", sample_count_);
  LOG_DEBUG("tensor_count=%u", tensor_count);

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

bool RecordIO::IsPrimary(size_t i) {
  return (configs_[i].level == 0) && (!configs_[i].is_aux_number);
}

void RecordIO::WriteSample(const std::unordered_map<std::string, Tensor*>& sample) {
  tensor_count_t count = names_.size();
  stream_->Write(&count, sizeof(count));

  tensor_dim_t batch_size = 1;
  for (auto i = 0; i < names_.size(); ++i) {
    const auto& name = names_[i];
    const auto iter = sample.find(name);
    CHECK(iter != sample.end());
    auto tensor = iter->second;

    if (IsPrimary(i)) {
      batch_size = tensor->shape()[0];
    }
    const auto& shape = tensor->shape();
    tensor_dim_count_t dim_count = shape.size();
    stream_->Write(&dim_count, sizeof(dim_count));

    for (const auto& dim : shape) {
      stream_->Write(&dim, sizeof(dim));
    }

    auto blob = tensor->mutable_blob();
    stream_->Write(blob->data(), blob->size());
  }
  sample_count_ += batch_size;
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

  LOG_DEBUG("sample_count=%u tensor_count=%u", sample_count_, names_.size());
  for (auto i = 0; i < names_.size(); ++i) {
    LOG_DEBUG("name=%s dtype=%d", names_[i].c_str(), configs_[i].type);
  }
}

std::vector<std::vector<Tensor*>> RecordIO::Split(std::vector<Tensor*>& raw, size_t split_size) {
  std::vector<Tensor*> id_num_tensor;
  for (auto i = 0; i < raw.size(); ++i) {
    if (configs_[i].is_aux_number) {
      CHECK(raw[i]->data_type() == kUInt32);
      id_num_tensor.resize(configs_[i].level + 1);
      id_num_tensor[configs_[i].level] = raw[i];
    }
  }
  
  std::vector<size_t> first_top(id_num_tensor.size());
  std::vector<size_t> second_bottom(id_num_tensor.size());
  size_t first_threshold = split_size;
  size_t second_threshold = split_size + 1;

  std::vector<Tensor*> new_id_num_tensor;  // The new split id num tensor
  for (auto i = 0; i < id_num_tensor.size(); ++i) {
    size_t prefix_sum0 = 0;
    for (auto i = 0; i < id_num_tensor[i]->mutable_blob()->size<uint32_t>(); ++i) {
      prefix_sum0 += id_num_tensor[i]->mutable_blob()->As<uint32_t>(i);
      if (first_threshold <= prefix_sum0) {
        first_top[i] = first_threshold;
        first_threshold = i + 1;
        break;
      }
    }
    size_t prefix_sum1 = 0;
    for (auto i = 0; i < id_num_tensor[i]->mutable_blob()->size<uint32_t>(); ++i) {
      prefix_sum1 += id_num_tensor[i]->mutable_blob()->As<uint32_t>(i);
      if (second_threshold <= prefix_sum1) {
        second_bottom[i] = second_threshold;
        second_threshold = i + 1;
        
        // reset id_num_tensor
        tensor_dim_t dim = id_num_tensor[i]->mutable_blob()->size<uint32_t>() - i;
        new_id_num_tensor.push_back(new Tensor({ dim }, kUInt32));
        for (auto j = i; j < id_num_tensor[i]->mutable_blob()->size<uint32_t>(); ++j) {
          new_id_num_tensor[i]->mutable_blob()->As<uint32_t>(j - i) =
              id_num_tensor[i]->mutable_blob()->As<uint32_t>(i);
        }
        new_id_num_tensor[i]->mutable_blob()->As<uint32_t>(0) = prefix_sum1 - second_bottom[i] + 1;

        dim = i + 1;
        id_num_tensor[i]->Reshape({ dim });
        id_num_tensor[i]->mutable_blob()->As<uint32_t>(i) -= (prefix_sum0 - first_top[i]);
        break;
      }
    }
  }

  if (first_top.empty()) {
    first_top.push_back(split_size);
    second_bottom.push_back(split_size + 1);
  } else {
    first_top.push_back(1);
    second_bottom.push_back(1);
  }

  std::vector<Tensor*> second;
  for (auto i = 0; i < raw.size(); ++i) {
    if (configs_[i].is_aux_number) {
      second.push_back(new_id_num_tensor[configs_[i].level]);
    } else {
      auto level = configs_[i].level;
      auto shape = raw[i]->shape();

      auto new_shape = shape;
      new_shape[0] = new_shape[0] - second_bottom[level] + 1;
      auto new_tensor = new Tensor(new_shape, raw[i]->data_type());
      DATA_TYPE_SWITCH(raw[i]->data_type(), DType, {
        memcpy(reinterpret_cast<DType*>(new_tensor->mutable_blob()->data()),
               reinterpret_cast<DType*>(raw[i]->mutable_blob()->data()) + 
                       ((second_bottom[level] - 1) * raw[i]->size()) / shape[0],
               new_tensor->size() * sizeof(DType));
      });

      second.push_back(new_tensor);

      shape[0] = first_top[level];
      raw[i]->Reshape(shape);
    }
  }

  std::vector<std::vector<Tensor*>> ret;
  ret.push_back(raw);
  ret.push_back(second);
  
  return ret;
}

}  // namespace hpps
