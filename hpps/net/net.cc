/*!
 * \file net.cc
 * \brief The net implementation
 */
#include "hpps/net/net.h"

#include <limits>
#include <mutex>

#include "hpps/common/configure.h"
#include "hpps/common/message.h"
#include "hpps/common/log.h"

#include "hpps/net/zmq_net.h"
#include "hpps/net/mpi_net.h"

namespace hpps {

HPPS_DEFINE_string(net_type, "zmq", "use zmq by default");

NetInterface* NetInterface::Get() {
  if (HPPS_CONFIG_net_type == "mpi") {
    static MPINetWrapper net_impl;
    return &net_impl;
  } else if (HPPS_CONFIG_net_type == "zmq") {
    static ZMQNetWrapper net_impl;
    return &net_impl;
  } else {
    LOG_FATAL("Unkown net type: %s", HPPS_CONFIG_net_type.c_str());
    return nullptr;
  }
}

}  // namespace hpps
