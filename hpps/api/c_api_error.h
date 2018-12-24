/*
 * \file c_api_error.h
 */
#ifndef H_CAPI_ERROR_H_
#define H_CAPI_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

void HPPS_GetLastErrorString(const char** msg);
void HPPS_SetLastErrorString(const char* format, ...);

#ifndef HPPS_SET_LAST_ERROR_STRING
#define HPPS_SET_LAST_ERROR_STRING(format, ...) \
HPPS_SetLastErrorString("[%s:%d] " format, __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  // H_CAPI_ERROR_H_
