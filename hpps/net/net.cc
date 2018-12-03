/*!
 * \file net.cc
 * \brief The net implementation
 */
#include "hpps/net/net.h"

#include <limits>
#include <mutex>

#include "hpps/common/message.h"
#include "hpps/common/log.h"

#include "hpps/net/zmq_net.h"
#include "hpps/net/mpi_net.h"

namespace hpps {

NetInterface* NetInterface::Get() {
#ifndef USE_MPI
  static ZMQNetWrapper net_impl;
  return &net_impl;
#else
  // Use MPI by default
  static MPINetWrapper net_impl;
  return &net_impl;
#endif
}

namespace net {
template <typename Typename>
void Allreduce(Typename* data, size_t elem_count) {
#ifdef USE_MPI
  CHECK(NetInterface::Get()->active());
  MPINetWrapper::Allreduce(data, elem_count);
#else
  Log::Fatal("Not implemented yet");
#endif
}

template void Allreduce<char>(char*, size_t);
template void Allreduce<int>(int*, size_t);
template void Allreduce<float>(float*, size_t);
template void Allreduce<double>(double*, size_t);

}  // namespace net

}  // namespace hpps
