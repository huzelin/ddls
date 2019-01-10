/*!
 * \file c_api_table.cc
 * \brief table interface
 */
#include "hpps/api/c_api_table.h"

#include "hpps/api/c_api_error.h"
#include "hpps/frame/table_interface.h"

using namespace hpps;

int TableWait(Handle handle, int id) {
  WorkerTable* worker_table = reinterpret_cast<WorkerTable*>(handle);
  if (worker_table != nullptr) {
    worker_table->Wait(id);
    return 0;
  } else {
    HPPS_SetLastErrorString("Is not a worker node, should not call table wait");
    return -1;
  }
}

int TableLoad(Handle handle, const char* uri) {
  WorkerTable* worker_table = reinterpret_cast<WorkerTable*>(handle);
  if (worker_table != nullptr) {
    worker_table->Load(uri);
    return 0;
  } else {
    HPPS_SetLastErrorString("Is not a worker node, should not call table load");
    return -1;
  }
}

int TableStore(Handle handle, const char* uri) {
  WorkerTable* worker_table = reinterpret_cast<WorkerTable*>(handle);
  if (worker_table != nullptr) {
    worker_table->Store(uri);
    return 0;
  } else {
    HPPS_SetLastErrorString("Is not a worker node, should not call table store");
    return -1;
  }
}
