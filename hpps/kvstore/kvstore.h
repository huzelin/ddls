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

  KVStore(size_t init_capacity, uint32_t value_len);
  virtual ~KVStore();

  Val* mutable_data() { return data_; }
  // Get offset
  void GetOffset(const Key* key, size_t key_size, int64_t* offset, bool immutable = false);
  void GetOffset(const Key& key, int64_t* offset, bool immutable = false);

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
