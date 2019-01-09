/*!
 * \file kvstore.h
 * \brief The kvstore
 */
#pragma once

#include <stdint.h>
#include <sys/types.h>

#include <vector>

namespace hpps {

template <typename Key, typename Val>
class KVStore {
 public:
  struct Node {
    Key key;
    int64_t offset;
  };
  const int kNodeSize = sizeof(KVStore<Key, Val>::Node);
  const int64_t kImmutableMask = 0x4000000000000000;

  explicit KVStore(size_t init_capacity, uint32_t value_len);
  virtual ~KVStore();

  Val* mutable_data() { return data_; }
  uint32_t value_len() const { return value_len_; }
  const Node* node() const { return node_; }

  // Get the key's offset, if immutable is true, the missing key's offset will
  // be -1, otherwise, a new offset will be allocated and new_data will be true
  void Get(const Key& key, size_t* index, int64_t* offset, bool& immutable, bool& new_data);

  // Get the key's value, if immutable is true, the missing key's value will
  // be nullptr, otherwise, a new address will be allocated and new_data will be
  // true.
  Val* Get(const Key& key, bool& immutable, bool& new_data);
  Val* Get(const Key& key);

  void Set(const Key& key, const Val* val, bool immutable);
  
  void Clear();
  void PrintDebug();

 protected:
  void AddBlackList();

  Node* node_;
  size_t node_size_;
  size_t node_capacity_;
  
  Val* data_;
  int64_t data_size_;
  int64_t data_capacity_;
  
  uint32_t value_len_;
};

}  // namespace hpps
