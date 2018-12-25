/*!
 * \file string_util.h
 * \brief The string util
 */
#pragma once

#include <string>
#include <vector>

namespace hpps {

inline std::vector<std::string> StrSplit(const std::string& raw_str, const char delim) {
  std::vector<std::string> splits;
  size_t i = 0, j = 0;
  for (; j < raw_str.length();) {
    if (raw_str.at(j) == delim) {
      splits.push_back(std::string(raw_str.c_str() + i, j - i));
      i = ++j;
    } else {
      ++j;
    }
  }
  splits.push_back(std::string(raw_str.c_str() + i, j - i));
  return splits;
} 

}  // namespace hpps
