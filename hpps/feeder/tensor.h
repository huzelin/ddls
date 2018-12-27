/*!
 * \file tensor.h
 * \brief The nd-dimension tensor
 */
#pragma once

#include <vector>
#include <memory>

#include "hpps/common/blob.h"
#include "hpps/feeder/types.h"

namespace hpps {

class Tensor {
 public:
  Tensor(const std::vector<tensor_dim_t>& shape,
         tensor_data_type_t data_type)
      : shape_(shape), data_type_(data_type) {
    size_ = 1;
    for (auto dim : shape_) {
      size_ *= dim;
    }
    blob_.reset(new Blob(TensorDataTypeSize(data_type_) * size_));
  }

  // Return mutable blob of tensor
  Blob* mutable_blob() { return blob_.get(); }
  // Return the shape of tensor
  const std::vector<tensor_dim_t> shape() const { return shape_; }
  // Return the size of tensor
  tensor_dim_t size() const { return size_; }
  // Return the data type
  tensor_data_type_t data_type() const { return data_type_; }

 protected:
  std::shared_ptr<Blob> blob_;
  std::vector<tensor_dim_t> shape_;
  tensor_dim_t size_;
  tensor_data_type_t data_type_;
};

}  // namespace hpps
