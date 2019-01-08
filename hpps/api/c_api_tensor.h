/*
 * \file c_api_feeder.h
 */
#ifndef H_CAPI_TENSOR_H_
#define H_CAPI_TENSOR_H_

#ifdef __cplusplus
extern "C" {
#include <sys/types.h>
#include <stdint.h>
#endif

// Definition of handle
typedef void* Handle;

// Tensor
int HPPS_TensorCreate(uint32_t dim, const uint32_t* shape, uint32_t type, Handle* out);
int HPPS_TensorShape(Handle handle, uint32_t* out_dim, const uint32_t** out_data);
int HPPS_TensorType(Handle handle, uint8_t* out);
int HPPS_TensorData(Handle handle, void** out);
int HPPS_TensorLoadData(Handle handle, size_t offset, void* data, size_t size);
int HPPS_TensorExportData(Handle handle, void* data);
int HPPS_TensorDestroy(Handle handle);


#ifdef __cplusplus
}
#endif

#endif  // H_CAPI_TENSOR_H_
