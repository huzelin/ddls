/*!
 * \file io.cc
 * \brief The io
 */
#include "hpps/common/io/io.h"
#include "hpps/common/io/hdfs_stream.h"
#include "hpps/common/io/local_stream.h"

namespace hpps {

Stream* StreamFactory::GetStream(const URI& uri,
  FileOpenMode mode) {
  std::string addr = uri.scheme + "://" + uri.host;
  if (instances_.find(addr) == instances_.end()) {
    if (uri.scheme == std::string("file"))
      instances_[addr] = std::shared_ptr<StreamFactory>(new LocalStreamFactory(uri.host));
    else if (uri.scheme == std::string("hdfs"))
      instances_[addr] = std::shared_ptr<StreamFactory>(new HDFSStreamFactory(uri.host));
    else Log::Fatal("Can not support the StreamFactory '%s'", uri.scheme.c_str());
  }
  return instances_[addr]->Open(uri, mode);
}

std::map<std::string, std::shared_ptr<StreamFactory> > StreamFactory::instances_;

}  // namespace hpps
