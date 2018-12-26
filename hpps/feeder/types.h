/*!
 * \file types.h
 * \brief The types in feeder module
 */
#pragma once

#include <stdint.h>
#include "hpps/common/log.h"

namespace hpps {

static const int kTensorNameLen = 32;

typedef uint64_t sample_count_t;
typedef uint16_t tensor_count_t;
typedef uint8_t  tensor_dim_count_t;

typedef uint32_t tensor_dim_t;
typedef uint8_t  tensor_data_type_t;

enum TensorDataType {
  kUInt32 = 0,
  kUInt64 = 1,
  kFloat32 = 2,
};

// Return the data type size.
inline size_t TensorDataTypeSize(tensor_data_type_t data_type) {
  switch (data_type) {
    case kUInt32:
      return sizeof(uint32_t);
    case kUInt64:
      return sizeof(uint64_t);
    case kFloat32:
      return sizeof(float);
    default:
      Log::Fatal("Unkown data_type=%d", data_type);
  }
}

#ifndef DATA_TYPE_SWITCH
#define DATA_TYPE_SWITCH(type, DType, ...)    \
    switch (type) {                           \
      case kUInt32:                           \
        {                                     \
          typedef uint32_t DType;             \
          {__VA_ARGS__}                       \
        }                                     \
        break;                                \
      case kUInt64:                           \
        {                                     \
          typedef uint64_t DType;             \
          {__VA_ARGS__}                       \
        }                                     \
        break;                                \
     case kFloat32:                           \
        {                                     \
          typedef float DType;                \
          {__VA_ARGS__}                       \
        }                                     \
        break;                                \
    }
#endif

}  // namespace hpps
