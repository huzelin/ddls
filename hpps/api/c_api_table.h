/*
 * \file c_api_table.h
 */
#ifndef H_CAPI_TABLE_H_
#define H_CAPI_TABLE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void* Handle;

int TableWait(Handle handle, int id);

#ifdef __cplusplus
}
#endif

#endif  // H_CAPI_TABLE_H_
