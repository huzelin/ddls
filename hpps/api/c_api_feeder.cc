/*
 * \file c_api_feeder.cc
 */

#include "hpps/api/c_api_feeder.h"

#include "hpps/feeder/feeder.h"
#include "hpps/feeder/record_io.h"

using namespace hpps;

// Record I/O
int HPPS_RecordIOCreate(int mode, Handle* record_io) {
  return 0;
}

int HPPS_WriteHeader(Handle record_io, int num, const char** name, const int** type) {
  return 0;
}

int HPPS_WriteSample(Handle record_io, int num, const char** name, Handle* tensor) {
  return 0;
}

int HPPS_WriteFinalize(Handle record_io) {
  return 0;
}

// Tensor
int HPPS_TensorShape(Handle tensor) {
  return 0;
}

int HPPS_TensorType(Handle tensor) {
  return 0;
}

// Feeder start and Plan scheduling.
int HPPS_FeederStart(int thread_num) {
  return 0;
}

int HPPS_FeederSchedule(Handle plan, int max_queue_size, Handle* iterator) {
  return 0;
}

// Plan maker creation
int HPPS_CreatePlanMaker(Handle* plan_maker) {
  return 0;
}

int HPPS_PlanMakerSetURI(Handle plan_maker, int num, const char** uri) {
  return 0;
}

int HPPS_PlanMakerSetEpoch(Handle plan_maker, int epoch) {
  return 0;
}

int HPPS_PlanMakerSetBatchSize(Handle plan_maker, int batch_size) {
  return 0;
}

int HPPS_PlanMakerMake(Handle plan_maker, Handle* plan) {
  return 0;
}

// Batch Iterator
int HPPS_BatchIteratorPop(Handle iterator, Handle* batch) {
  return 0;
}

// Batch operation
int HPPS_BatchGetTensorFromKey(Handle batch, const char* key, Handle* tensor) {
  return 0;
}

int HPPS_BatchGetTensorFromIndex(Handle batch, int index, Handle* tensor) {
  return 0;
}

int HPPS_BatchDestroy(Handle batch) {
  return 0;
}
