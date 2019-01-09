/*!
 * \file kv_table.cc
 * \brief The kv table
 */
#include "hpps/table/kv_table.h"

#include "hpps/common/io/io.h"
#include "hpps/common/log.h"
#include "hpps/updater/updater.h"

namespace hpps {

template <typename Key, typename Val>
void KVWorkerTable<Key, Val>::Get(const Key* key, size_t size, Tensor* val) {
  WorkerTable::Get(Blob(key, sizeof(Key) * size));
  
  CHECK(val->data_type() == DataType<Val>::kFlag);
  tensor_dim_t dim0 = size;
  tensor_dim_t dim1 = store_->value_len();
  val->Reshape({ dim0, dim1 });
  
  Val* addr = reinterpret_cast<Val*>(val->mutable_blob()->data());
  for (auto i = 0; i < size; ++i) {
    auto src = store_->Get(key[i]);
    memcpy(addr, src, store_->value_len() * sizeof(Val));
    addr += store_->value_len();
  }
}

template <typename Key, typename Val>
int KVWorkerTable<Key, Val>::GetAsync(const Key* key, size_t size) {
  return WorkerTable::GetAsync(Blob(key, sizeof(Key) * size));
}

template <typename Key, typename Val>
void KVWorkerTable<Key, Val>::Add(const Key* key, size_t key_size,
                                  const Val* val, size_t val_size, const AddOption* option) {
  CHECK(key_size * store_->value_len() == val_size);
  Blob keys_blob(key, sizeof(Key) * key_size);
  Blob vals_blob(val, sizeof(Val) * val_size);
  WorkerTable::Add(keys_blob, vals_blob, option);
}

template <typename Key, typename Val>
int KVWorkerTable<Key, Val>::AddAsync(const Key* key, size_t key_size,
                                      const Val* val, size_t val_size, const AddOption* option) {
  CHECK(key_size * store_->value_len() == val_size);
  Blob keys_blob(key, sizeof(Key) * key_size);
  Blob vals_blob(val, sizeof(Val) * val_size);
  return WorkerTable::AddAsync(keys_blob, vals_blob, option);
}

template <typename Key, typename Val>
KVServerTable<Key, Val>::KVServerTable(const KVTableOption<Key, Val>& kvtable_option) : ServerTable(kvtable_option.ps_mode) {
  ParamInitializer<Val>::ResetRandomOption(kvtable_option.random_option);
  store_.reset(new KVStore<Key, Val>(kvtable_option.init_capacity, kvtable_option.value_len));
  updater_.reset(Updater<Val>::GetUpdater(kvtable_option.init_capacity, kvtable_option.solver));
  LOG_INFO("create KVTable with %u elements(solver=%s ps_mode=%s).", 
           kvtable_option.init_capacity, kvtable_option.solver.c_str(), kvtable_option.ps_mode.c_str());
}

template <typename Key, typename Val>
void KVServerTable<Key, Val>::Store(Stream* stream) {

}

template <typename Key, typename Val>
void KVServerTable<Key, Val>::Load(Stream*) {

}

template class KVWorkerTable<uint32_t, float>;
template class KVServerTable<uint32_t, float>;
template class KVWorkerTable<uint64_t, float>;
template class KVServerTable<uint64_t, float>;

}  // namespace hpps
