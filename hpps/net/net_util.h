/*!
 * \file net_util.h
 * \brief The net utility
 */
#pragma once

#include <string>
#include <unordered_set>

namespace hpps {
namespace net {

void GetLocalIPAddress(std::unordered_set<std::string>* result);

}  // namespace net
}  // namespace hpps
