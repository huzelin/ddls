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

template <typename DType>
struct DataType;

template <>
struct DataType<uint32_t> {
  static const TensorDataType kFlag = kUInt32;
};

template <>
struct DataType<uint64_t> {
  static const TensorDataType kFlag = kUInt64;
};

template <>
struct DataType<float> {
  static const TensorDataType kFlag = kFloat32;
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
      LOG_FATAL("Unkown data_type=%d", data_type);
  }
}

#ifndef ID_TYPE_SWITCH
#define ID_TYPE_SWITCH(type, DType, ...)      \
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
      default:                                \
        LOG_FATAL("Unkown dtype %d", type);   \
    }
#endif

#ifndef VALUE_TYPE_SWITCH
#define VALUE_TYPE_SWITCH(type, DType, ...)    \
    switch (type) {                           \
     case kFloat32:                           \
        {                                     \
          typedef float DType;                \
          {__VA_ARGS__}                       \
        }                                     \
        break;                                \
      default:                                \
        LOG_FATAL("Unkown dtype %d", type);   \
    }
#endif

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
      default:                                \
        LOG_FATAL("Unkown dtype %d", type);   \
    }
#endif

}  // namespace hpps
