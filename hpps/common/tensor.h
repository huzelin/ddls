/*!
 * \file tensor.h
 * \brief The nd-dimension tensor
 */
#pragma once

#include <vector>
#include <memory>
#include <sstream>

#include "hpps/common/blob.h"
#include "hpps/common/types.h"

namespace hpps {

class Tensor {
 public:
  Tensor(const std::vector<tensor_dim_t>& shape,
         tensor_data_type_t data_type) : data_type_(data_type), size_(0), capacity_(0) {
    Reshape(shape);
  }

  void Reshape(const std::vector<tensor_dim_t>& shape) {
    shape_ = shape;
    auto size = 1;
    for (auto dim : shape_) size *= dim;
    if (size > capacity_) {
      capacity_ = size;
      blob_.reset(new Blob(TensorDataTypeSize(data_type_) * capacity_));
    }
    size_ = size;
  }

  // Return mutable blob of tensor
  Blob* mutable_blob() { return blob_.get(); }
  // Return the shape of tensor
  const std::vector<tensor_dim_t>& shape() const { return shape_; }
  // Return the size of tensor
  size_t size() const { return size_; }
  // Return the data type
  tensor_data_type_t data_type() const { return data_type_; }

  void PrintDebug() {
    auto row = shape()[0];
    auto col = size() / row;
    
    std::stringstream ss;
    for (auto i = 0; i < row; ++i) {
      for (auto j = 0; j < col; ++j) {
        DATA_TYPE_SWITCH(data_type(), DType, {               
          ss << mutable_blob()->As<DType>(i * col + j) << " ";
        });
      }
      ss << std::endl;
    }
    LOG_INFO("%s", ss.str().c_str());
  }

 protected:
  std::shared_ptr<Blob> blob_;
  std::vector<tensor_dim_t> shape_;
  size_t size_, capacity_;
  tensor_data_type_t data_type_;
};

}  // namespace hpps
