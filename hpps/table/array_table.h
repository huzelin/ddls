/*!
 * \file array_table.h
 * \brief The array table
 */
#include "hpps/frame/table_interface.h"
#include "hpps/common/log.h"

namespace hpps {

template<typename EleType>
struct ArrayTableOption;

template <typename T>
class ArrayWorker : public WorkerTable {
 public:
  explicit ArrayWorker(const ArrayTableOption<T> &option);
  // std::vector<T>& raw() { return table_; }

  // Get all element, data is user-allocated memory, Blocking IO
  void Get(T* data, size_t size);
  // Non-blocking IO
  int GetAsync(T* data, size_t size);

  // Add all element
  void Add(T* data, size_t size, const AddOption* option = nullptr);
  int AddAsync(T* data, size_t, const AddOption* option = nullptr);

  int Partition(const std::vector<Blob>& kv,
    MsgType partition_type,
    std::unordered_map<int, std::vector<Blob> >* out) override;

  void ProcessReplyGet(std::vector<Blob>& reply_data) override;
 
 private:
  explicit ArrayWorker(size_t size);
  
  T* data_; // not owned
  size_t size_;
  int num_server_;
  std::vector<size_t> server_offsets_;
};

template <typename T>
class Updater;

// The storage is a continuous large chunk of memory
template <typename T>
class ArrayServer : public ServerTable, public ParamInitializer<T> {
 public:
  explicit ArrayServer(const ArrayTableOption<T> &option);

  void ProcessAdd(const std::vector<Blob>& data) override;

  void ProcessGet(const std::vector<Blob>& data,
                  std::vector<Blob>* result) override;

  void Store(Stream* s) override;
  void Load(Stream* s) override;
 
 private:
  explicit ArrayServer(size_t size, const std::string& solver);

  int32_t server_id_;
  std::vector<T> storage_;
  std::shared_ptr<Updater<T>> updater_;
  size_t size_; // number of element with type T
};

template<typename T>
struct ArrayTableOption {
  explicit ArrayTableOption(size_t s, const char* solver, const char* ps_mode)
      : size(s), solver(solver), ps_mode(ps_mode) { }
  
  size_t size;
  std::string solver;
  std::string ps_mode;
  
  RandomOption random_option;

  DEFINE_TABLE_TYPE(T, ArrayWorker, ArrayServer);
};

}  // namespace hpps
