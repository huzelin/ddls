/*!
 * \file hdfs_stream.h
 * \brief The hdfs stream
 */
#pragma once

#include "hpps/common/io/io.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "hdfs.h"

#include <cstring>
#include <cstdio>
#include <cerrno>
#include <cassert>

#include <algorithm>
#include <memory>

namespace hpps {

class HDFSStream : public Stream {
 public:
  HDFSStream(hdfsFS fs, const URI& uri,  FileOpenMode mode);

  virtual ~HDFSStream(void) override;

  /*!
  * \brief write data to a file
  * \param buf pointer to a memory buffer
  * \param size data size
  */
  virtual void Write(const void *buf, size_t size) override;


  /*!
  * \brief read data from Stream
  * \param buf pointer to a memory buffer
  * \param size the size of buf
  */
  virtual size_t Read(void *buf, size_t size) override;

  virtual bool Good() override;

 private:
  bool is_good_;
  hdfsFS fs_;
  hdfsFile fp_;
  std::string path_;
  std::string mode_;
};

class HDFSStreamFactory : public StreamFactory {
 public:
  explicit HDFSStreamFactory(const std::string& host);
  virtual ~HDFSStreamFactory(void) override;

  /*!
  * \brief create a Stream
  * \param path the path of the file
  * \param mode "w" - create an empty file to store data;
  *             "a" - open the file to append data to it
  *             "r" - open the file to read
  * \return the Stream which is used to write or read data
  */
  virtual Stream* Open(const URI& uri,
                       FileOpenMode mode) override;

  virtual void Close() override;

 private:
  std::string namenode_;
  hdfsFS fs_;
};

}  // namespace hpps
