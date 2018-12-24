/*
 * \file c_api_error.cc
 */
#include "hpps/api/c_api_error.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string>

#include "hpps/common/thread_local.h"

using namespace hpps;

const int kMaxBufSize = 4 * 1024 * 1024;

struct StringEntry {
  char last_string[kMaxBufSize];
};

typedef hpps::ThreadLocalStore<StringEntry> ThreadLocalStringStore;

void HPPS_GetLastErrorString(const char** msg) {
  *msg = ThreadLocalStringStore::Get()->last_string;
}

void HPPS_SetLastErrorString(const char* format, ...) {
  va_list val;
  va_start(val, format);
  char* buf = ThreadLocalStringStore::Get()->last_string;
  snprintf(buf, kMaxBufSize, format, val);
  va_end(val);
}
