/*!
 * \file types.h
 * \brief The types in feeder module
 */
#pragma once

#include <stdint.h>

namespace hpps {

static const int kTensorNameLen = 32;

typedef uint64_t feeder_sample_count_t;
typedef uint16_t feeder_tensor_count_t;
typedef uint8_t  feeder_tensor_dim_count_t;
typedef uint32_t feeder_tensor_dim_t;
typedef uint8_t  feeder_tensor_type_t;

}  // namespace hpps
