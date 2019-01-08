/*!
 * \file kvstore.cc
 * \brief The kvstore implementation
 */
#include "hpps/kvstore/kvstore.h"

#include <string.h>

#include "hpps/common/allocator.h"
#include "hpps/common/log.h"

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
void KVStore<Key, Val>::Get(const Key& key, size_t* node_index,
                            int64_t* offset, bool& immutable, bool& new_data) {
  new_data = false;
  size_t index = key & (node_capacity_ - 1);
  int probe_num = kMaxProbeNum;
  while (index < node_capacity_ && probe_num--) {
    if (node_[index].offset < 0) {
      if (immutable) {
        *offset = -1;
      } else {
        // Add one new Entry.
        node_[index].key = key;
        node_[index].offset = data_size_;
        
        *offset = data_size_;
        *node_index = index;
        new_data = true;
        
        data_size_ += value_len_;
        ++node_size_;
      }
      return;
    } else if (node_[index].key == key) {
      *offset = node_[index].offset;
      *node_index = index;
      if ((*offset & kImmutableMask) != 0) {
        *offset &= ~(kImmutableMask);
        immutable = true;
      }
      return;
    }
    index = (index + 1) & (node_capacity_ - 1);
  }
  // Need to expand the space
  LOG_FATAL("Please set init_capacity larger");
}

template <typename Key, typename Val>
Val* KVStore<Key, Val>::Get(const Key& key, bool& immutable, bool& new_data) {
  int64_t offset;
  size_t node_index;
  Get(key, &node_index, &offset, immutable, new_data);
  if (offset < 0) {
    return nullptr;
  } else {
    return mutable_data() + offset;
  }
}

template <typename Key, typename Val>
Val* KVStore<Key, Val>::Get(const Key& key) {
  bool immutable = true;
  bool new_data;
  return Get(key, immutable, new_data);
}

template <typename Key, typename Val>
void KVStore<Key, Val>::Set(const Key& key, const Val* val, bool immutable) {
  int64_t offset;
  size_t node_index;
  bool new_data;
  bool immu = false;
  Get(key, &node_index, &offset, immu, new_data);
  memcpy(mutable_data() + offset, val, value_len_ * sizeof(Val));

  if (immutable) {
    node_[node_index].offset |= kImmutableMask;
  } else {
    node_[node_index].offset &= ~(kImmutableMask);
  }
}

template <typename Key, typename Val>
void KVStore<Key, Val>::Clear() {
  for (auto i = 0; i < node_size_; ++i) {
    node_[i].offset = -1;
  }
  node_size_ = 0;
  data_size_ = 0;
}

template <typename Key, typename Val>
void KVStore<Key, Val>::PrintDebug() {
  for (auto i = 0; i < node_size_; ++i) {
    if (node_[i].offset != -1) {
      LOG_INFO("Key=%u offset=%d", node_[i].key, node_[i].offset);
    }
  }
}

template class KVStore<uint32_t, float>;
template class KVStore<uint64_t, float>;

}  // namespace hpps
