/*!
 * \file kvstore.cc
 * \brief The kvstore implementation
 */
#include "hpps/kvstore/kvstore.h"

#include "hpps/common/allocator.h"

namespace {
int kMaxProbeNum = 32;
}  // namespace

namespace hpps {

template <typename Key, typename Val>
KVStore<Key, Val>::KVStore(size_t init_capacity, uint32_t value_len) : value_len_(value_len) {
  node_capacity_ = 1;
  while (node_capacity_ < init_capacity) node_capacity_ *= 2;
  node_ = reinterpret_cast<Node*>(AlignMalloc(node_capacity_ * kNodeSize));
  for (auto i = 0; i < node_capacity_; ++i) {
    node_[i].offset = -1;
  }
  node_size_ = 0;

  data_size_ = 0;
  data_capacity_ = node_capacity_ * value_len_;
  data_ = reinterpret_cast<Val*>(AlignMalloc(data_capacity_ * sizeof(Val)));
}

template <typename Key, typename Val>
KVStore<Key, Val>::~KVStore() {
  AlignFree(reinterpret_cast<char*>(node_));
  AlignFree(reinterpret_cast<char*>(data_));
}

template <typename Key, typename Val>
void KVStore<Key, Val>::GetOffset(const Key* key, size_t key_size, int64_t* offset, bool immutable) {
  for (auto i = 0; i < key_size; ++i) {
    GetOffset(key[i], offset + i, immutable);
  }
}

template <typename Key, typename Val>
void KVStore<Key, Val>::GetOffset(const Key& key, int64_t* offset, bool immutable) {
  size_t index = key & (node_capacity_ - 1);
  int probe_num = kMaxProbeNum;
  while (index < node_capacity_ && probe_num--) {
    if (node_[index].key == key) {
      *offset = node_[index].offset;
      return;
    } else if (node_[index].offset < 0) {
      if (immutable) *offset = -1;
      else {
        node_[index].key = key;
        node_[index].offset = data_size_;
        *offset = data_size_;
        data_size_ += value_len_;
        ++node_size_;
      }
      return;
    }
    index = (index + 1) & (node_capacity_ - 1);
  }
  // Need to expand the space
}

template class KVStore<uint64_t, float>;

}  // namespace hpps
