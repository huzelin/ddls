/*
 * \file c_api_array_table.h
 */
#ifndef H_CAPI_ARRAY_TABLE_H_
#define H_CAPI_ARRAY_TABLE_H_

#ifdef __cplusplus
extern "C" {
#include <stdint.h>
#include <sys/types.h>
#endif

typedef void* Handle;

int ArrayTableGet(Handle handle, Handle tensor);
int ArrayTableGetAsync(Handle handle, Handle tensor, int* id);

int ArrayTableAdd(Handle handle, Handle grad);
int ArrayTableAddAsync(Handle handle, Handle grad, int* id);

int CreateArrayTable(const char* solver,
                     const char* ps_mode,
                     size_t size,
                     int type,
                     int num,
                     const char** key,
                     const char** value,
                     Handle* out);

#ifdef __cplusplus
}
#endif

#endif  // H_CAPI_ARRAY_TABLE_H_
