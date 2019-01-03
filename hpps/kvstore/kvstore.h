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

  explicit KVStore(size_t init_capacity, uint32_t value_len);
  virtual ~KVStore();

  Val* mutable_data() { return data_; }
  uint32_t value_len() const { return value_len_; }
  const Node* node() const { return node_; }

  void Get(const Key* key, size_t key_size, int64_t* offset, bool immutable = false, bool* new_data = nullptr);
  void Get(const Key& key, int64_t* offset, bool immutable = false, bool* new_data = nullptr);

  Val* Get(const Key& key, bool immutable = false, bool* new_data = nullptr);

  void Set(const Key* key, size_t key_size, const Val* val);
  void Set(const Key& key, const Val* val);
  void Clear();

 protected:
  Node* node_;
  size_t node_size_;
  size_t node_capacity_;
  
  Val* data_;
  int64_t data_size_;
  int64_t data_capacity_;
  
  uint32_t value_len_;
};

}  // namespace hpps
