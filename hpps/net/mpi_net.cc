/*!
 * \file mpi_net.cc
 * \brief The mpi net interface implementation
 */
#include "hpps/net/mpi_net.h"

namespace hpps {

template void MPINetWrapper::Allreduce<char>(char*, size_t);
template void MPINetWrapper::Allreduce<int>(int*, size_t);
template void MPINetWrapper::Allreduce<float>(float*, size_t);
template void MPINetWrapper::Allreduce<double>(double*, size_t);

}  // namespace hpps
