/*
 * \file c_api_feeder.h
 */
#ifndef H_CAPI_FEEDER_H_
#define H_CAPI_FEEDER_H_

#ifdef __cplusplus
extern "C" {
#include <sys/types.h>
#include <stdint.h>
#endif

// Definition of handle
typedef void* Handle;

// Record I/O
int HPPS_RecordIOCreate(const char* uri, int mode, Handle* record_io);
int HPPS_RecordIOWriteHeader(Handle record_io, int num, const char** name, const int* type);
int HPPS_RecordIOWriteSample(Handle record_io, int num, const char** name, Handle* tensor);
int HPPS_RecordIOWriteFinalize(Handle record_io);
int HPPS_RecordIODestroy(Handle handle);

// Tensor
int HPPS_TensorCreate(uint32_t dim, const uint32_t* shape, uint32_t type, Handle* out);
int HPPS_TensorShape(Handle handle, uint32_t* out_dim, const uint32_t** out_data);
int HPPS_TensorType(Handle handle, uint8_t* out);
int HPPS_TensorData(Handle handle, void** out);
int HPPS_TensorLoadData(Handle handle, size_t offset, void* data, size_t size);
int HPPS_TensorExportData(Handle handle, void* data);
int HPPS_TensorDestroy(Handle handle);

// Feeder start and Plan scheduling.
int HPPS_FeederStart(int thread_num);
int HPPS_FeederSchedule(Handle plan, int max_queue_size, Handle* iterator);
int HPPS_FeederStop();

// Plan maker creation
int HPPS_PlanMakerCreate(Handle* plan_maker);
int HPPS_PlanMakerSetURI(Handle plan_maker, int num, const char** uri);
int HPPS_PlanMakerSetEpoch(Handle plan_maker, int epoch);
int HPPS_PlanMakerSetBatchSize(Handle plan_maker, int batch_size);
int HPPS_PlanMakerMake(Handle plan_maker, Handle* plan);

// Batch Iterator
int HPPS_BatchIteratorPop(Handle iterator, Handle* batch);

// Batch operation
int HPPS_BatchGetTensorFromKey(Handle batch, const char* key, Handle* tensor);
int HPPS_BatchGetTensorFromIndex(Handle batch, int index, Handle* tensor);
int HPPS_BatchDestroy(Handle batch);

// Utils
int HPPS_Num2Indices(Handle num_handle, Handle* indices);

#ifdef __cplusplus
}
#endif

#endif  // H_CAPI_FEEDER_H_
