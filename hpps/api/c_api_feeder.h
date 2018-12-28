/*
 * \file c_api_feeder.h
 */
#ifndef H_CAPI_FEEDER_H_
#define H_CAPI_FEEDER_H_

#ifdef __cplusplus
extern "C" {
#include <stdint.h>
#endif

// Definition of handle
typedef void* Handle;

// Record I/O
int HPPS_RecordIOCreate(const char* uri, int mode, Handle* record_io);
int HPPS_WriteHeader(Handle record_io, int num, const char** name, const int* type);
int HPPS_WriteSample(Handle record_io, int num, const char** name, Handle* tensor);
int HPPS_WriteFinalize(Handle record_io);

// Tensor
int HPPS_TensorShape(Handle handle, uint32_t* out_dim, const uint32_t** out_data);
int HPPS_TensorType(Handle handle, uint8_t* out);
int HPPS_TensorData(Handle handle, void** out);

// Feeder start and Plan scheduling.
int HPPS_FeederStart(int thread_num);
int HPPS_FeederSchedule(Handle plan, int max_queue_size, Handle* iterator);

// Plan maker creation
int HPPS_CreatePlanMaker(Handle* plan_maker);
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

#ifdef __cplusplus
}
#endif

#endif  // H_CAPI_FEEDER_H_
