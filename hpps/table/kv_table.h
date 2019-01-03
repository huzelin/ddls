/*!
 * \file kv_table.h
 * \brief The kv table
 */
#pragma once

#include "hpps/frame/table_interface.h"
#include "hpps/frame/zoo.h"
#include "hpps/common/log.h"

#include <unordered_map>
#include <vector>

#include "hpps/kvstore/kvstore.h"
#include "hpps/updater/updater.h"

namespace hpps {

template <typename Key, typename Val>
struct KVTableOption;

// A distributed shared std::unordered_map<Key, Val> table
// Key, Val should be the basic type
template <typename Key, typename Val>
class KVWorkerTable : public WorkerTable {
 public:
  explicit KVWorkerTable(const KVTableOption<Key, Val>& option) {
    store_.reset(new KVStore<Key, Val>(option.init_capacity, option.value_len));
  }

  void Get(Key key) { WorkerTable::Get(Blob(&key, sizeof(Key))); }

  void Get(std::vector<Key>& keys) {
    WorkerTable::Get(Blob(&keys[0], sizeof(Key) * keys.size()));
  }

  void Add(Key key, Val* value, size_t size) {
    CHECK(size == store_->value_len());
    WorkerTable::Add(Blob(&key, sizeof(Key)), Blob(value, sizeof(Val) * size));
  }

  void Add(std::vector<Key>& keys, std::vector<Val>& vals) {
    CHECK(keys.size() * store_->value_len() == vals.size());
    Blob keys_blob(&keys[0], sizeof(Key) * keys.size());
    Blob vals_blob(&vals[0], sizeof(Val) * vals.size());
    WorkerTable::Add(keys_blob, vals_blob);
  }

  KVStore<Key, Val>& raw() { return *(store_.get()); }
   
  int Partition(const std::vector<Blob>& kv, 
    MsgType, std::unordered_map<int, std::vector<Blob> >* out) override {
    CHECK(kv.size() == 1 || kv.size() == 2);
    CHECK_NOTNULL(out);
    std::unordered_map<int, int> counts;
    Blob keys = kv[0];
    for (int i = 0; i < keys.size<Key>(); ++i) { // iterate as type Key
      int dst = static_cast<int>(keys.As<Key>(i) % Zoo::Get()->num_servers());
      ++counts[dst];
    }
    for (auto& it : counts) { // Allocate memory
      std::vector<Blob>& vec = (*out)[it.first];
      vec.push_back(Blob(it.second * sizeof(Key)));
      if (kv.size() == 2) vec.push_back(Blob(it.second * sizeof(Val) * store_->value_len()));
    }
    counts.clear();
    for (int i = 0; i < keys.size<Key>(); ++i) {
      int dst = static_cast<int>(keys.As<Key>(i) % Zoo::Get()->num_servers());
      (*out)[dst][0].As<Key>(counts[dst]) = keys.As<Key>(i);
      if (kv.size() == 2) { 
        memcpy(reinterpret_cast<Val*>((*out)[dst][1].data()) + (counts[dst] * store_->value_len()),
               reinterpret_cast<Val*>(kv[1].data()) + i * store_->value_len(),
               store_->value_len() * sizeof(Val));
      }
      ++counts[dst];
    }
    return static_cast<int>(out->size());
  }

  void ProcessReplyGet(std::vector<Blob>& data) override {
    CHECK(data.size() == 2);
    Blob keys = data[0], vals = data[1];
    CHECK(keys.size<Key>() * store_->value_len() == vals.size<Val>());
    for (int i = 0; i < keys.size<Key>(); ++i) {
      store_->Set(keys.As<Key>(i),
                  reinterpret_cast<Val*>(vals.data()) + i * store_->value_len());
    }
  }
 
 private:
  std::shared_ptr<KVStore<Key, Val>> store_;
};

template <typename Key, typename Val>
class KVServerTable : public ServerTable, ParamInitializer<Val> {
 public:
  explicit KVServerTable(const KVTableOption<Key, Val>& option);

  void ProcessGet(const std::vector<Blob>& data, 
                  std::vector<Blob>* result) override {
    CHECK(data.size() == 1);
    CHECK_NOTNULL(result);
    Blob keys = data[0];
    result->push_back(keys); // also push the key
    result->push_back(Blob(keys.size<Key>() * sizeof(Val) * store_->value_len()));
    Blob& vals = (*result)[1];
    bool immutable = false;
    for (int i = 0; i < keys.size<Key>(); ++i) {
      bool new_data;
      auto addr = store_->Get(keys.As<Key>(i), immutable, &new_data);
      if (addr != nullptr) {
        if (new_data) {
          this->random_.Gen(addr, store_->value_len());
        }
        memcpy(reinterpret_cast<Val*>(vals.data()) + i * store_->value_len(),
               addr, sizeof(Val) * store_->value_len());
      } else {
        memset(reinterpret_cast<Val*>(vals.data()) + i * store_->value_len(),
               0, sizeof(Val) * store_->value_len());
      }
    }
  }

  void ProcessAdd(const std::vector<Blob>& data) override {
    CHECK(data.size() == 2 || data.size() == 3);
    Blob keys = data[0], vals = data[1];
    AddOption option;
    if (data.size() == 3)
      option.CopyFrom(data[2].data(), data[2].size());
  
    CHECK(keys.size<Key>() * store_->value_len() == vals.size<Val>());
    bool immutable = false;
    for (int i = 0; i < keys.size<Key>(); ++i) {
      bool new_data;
      auto addr = store_->Get(keys.As<Key>(i), immutable, &new_data);
      updater_->Update(store_->value_len(), addr,
                       reinterpret_cast<Val*>(vals.data()) + i * store_->value_len(),
                       &option);
    }
  }

  void Store(Stream*) override; 
  void Load(Stream*) override;
 
 private:
  std::shared_ptr<KVStore<Key, Val>> store_;
  std::shared_ptr<Updater<Val>> updater_;
};

template <typename Key, typename Val>
struct KVTableOption {
  explicit KVTableOption(size_t init_capacity, uint32_t value_len)
      : init_capacity(init_capacity / Zoo::Get()->num_servers()), value_len(value_len) { }

  size_t init_capacity;
  uint32_t value_len;
  RandomOption random_option;

  typedef KVWorkerTable<Key, Val> WorkerTableType;
  typedef KVServerTable<Key, Val> ServerTableType;
};

}  // namespace hpps
