/*!
* \file local_file_sys.h
* \brief the implement of local io interface.
*/
#include "hpps/common/io/io.h"

namespace hpps {

class LocalStream : public Stream {
 public:
  LocalStream(const URI& uri, FileOpenMode mode);
  virtual ~LocalStream(void) override;

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
  FILE *fp_;
  std::string path_;
};

class LocalStreamFactory : public StreamFactory {
 public:
  LocalStreamFactory(const std::string& host);
  ~LocalStreamFactory(void) override;

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
  std::string host_;
};

}  // namespace hpps
