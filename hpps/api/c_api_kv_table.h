/*
 * \file c_api_kv_table.h
 */
#ifndef H_CAPI_KV_TABLE_H_
#define H_CAPI_KV_TABLE_H_

#ifdef __cplusplus
extern "C" {
#include <stdint.h>
#include <sys/types.h>
#endif

typedef void* Handle;

int KVTableGet(Handle handle, Handle key, Handle tensor);
int KVTableGetAsync(Handle handle, Handle key, Handle tensor, int* id);

int KVTableAdd(Handle handle, Handle key, Handle grad);
int KVTableAddAsync(Handle handle, Handle key, Handle grad, int* id);

int CreateKVTable(const char* solver,
                  size_t capacity,
                  int value_len,
                  int key_type,
                  int value_type,
                  int num,
                  const char** key,
                  const char** value,
                  Handle* out);

#ifdef __cplusplus
}
#endif

#endif  // H_CAPI_KV_TABLE_H_
