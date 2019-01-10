/*!
 * \file worker.cc
 * \brief The worker actor
 */
#include "hpps/frame/worker.h"

#include <vector>

#include "hpps/common/dashboard.h"
#include "hpps/common/queue.h"
#include "hpps/common/log.h"
#include "hpps/frame/zoo.h"

namespace hpps {

Worker::Worker() : Actor(actor::kWorker) {
  RegisterHandler(MsgType::Request_Get, std::bind(
    &Worker::ProcessGet, this, std::placeholders::_1));
  RegisterHandler(MsgType::Request_Add, std::bind(
    &Worker::ProcessAdd, this, std::placeholders::_1));
  RegisterHandler(MsgType::Reply_Get, std::bind(
    &Worker::ProcessReplyGet, this, std::placeholders::_1));
  RegisterHandler(MsgType::Reply_Add, std::bind(
    &Worker::ProcessReplyAdd, this, std::placeholders::_1));

  RegisterHandler(MsgType::Load_Model, std::bind(
    &Worker::ProcessLoadModel, this, std::placeholders::_1));
  RegisterHandler(MsgType::Store_Model, std::bind(
    &Worker::ProcessStoreModel, this, std::placeholders::_1));
  RegisterHandler(MsgType::Reply_Load_Model, std::bind(
    &Worker::ProcessReplyLoadModel, this, std::placeholders::_1));
  RegisterHandler(MsgType::Reply_Store_Model, std::bind(
    &Worker::ProcessReplyStoreModel, this, std::placeholders::_1));
}

int Worker::RegisterTable(WorkerTable* worker_table) {
  CHECK_NOTNULL(worker_table);
  int id = static_cast<int>(cache_.size());
  cache_.push_back(worker_table);
  return id;
}

void Worker::ProcessGet(MessagePtr& msg) {
  MONITOR_BEGIN(WORKER_PROCESS_GET)
  int table_id = msg->table_id();
  int msg_id = msg->msg_id();
  std::unordered_map<int, std::vector<Blob>> partitioned_key;
  int num = cache_[table_id]->Partition(msg->data(),
                                        MsgType::Request_Get, 
                                        &partitioned_key);
  cache_[table_id]->Reset(msg_id, num);

  for (auto& it : partitioned_key) {
    MessagePtr new_msg(new Message());
    new_msg->set_src(Zoo::Get()->rank());
    new_msg->set_dst(Zoo::Get()->server_id_to_rank(it.first));
    new_msg->set_type(MsgType::Request_Get);
    new_msg->set_msg_id(msg_id);
    new_msg->set_table_id(table_id);
    new_msg->set_data(it.second);
    SendTo(actor::kCommunicator, new_msg);
  }
  MONITOR_END(WORKER_PROCESS_GET)
}

void Worker::ProcessAdd(MessagePtr& msg) {
  MONITOR_BEGIN(WORKER_PROCESS_ADD)
  int table_id = msg->table_id();
  int msg_id = msg->msg_id();
  std::unordered_map<int, std::vector<Blob>> partitioned_kv;
  CHECK_NOTNULL(msg.get());
  CHECK(!msg->data().empty());
  int num = cache_[table_id]->Partition(msg->data(), 
                                        MsgType::Request_Add, 
                                        &partitioned_kv);
  cache_[table_id]->Reset(msg_id, num);

  for (auto& it : partitioned_kv) {
    MessagePtr kv_msg(new Message());
  	kv_msg->set_src(Zoo::Get()->rank());
  	kv_msg->set_dst(Zoo::Get()->server_id_to_rank(it.first));
	  kv_msg->set_type(MsgType::Request_Add);
	  kv_msg->set_msg_id(msg_id);
	  kv_msg->set_table_id(table_id);
	  kv_msg->set_data(it.second);
    SendTo(actor::kCommunicator, kv_msg);
  }
  MONITOR_END(WORKER_PROCESS_ADD)
}

void Worker::ProcessReplyGet(MessagePtr& msg) {
  MONITOR_BEGIN(WORKER_PROCESS_REPLY_GET)
  int table_id = msg->table_id();
  cache_[table_id]->ProcessReplyGet(msg->data());
  cache_[table_id]->Notify(msg->msg_id());
  MONITOR_END(WORKER_PROCESS_REPLY_GET)
}

void Worker::ProcessReplyAdd(MessagePtr& msg) {
  cache_[msg->table_id()]->Notify(msg->msg_id());
}

void Worker::ProcessLoadModel(MessagePtr& msg) {
  std::string file = msg->data()[0].data(); 
  MONITOR_BEGIN(WORKER_PROCESS_LOAD_MODEL)
  for (auto i = 0; i < Zoo::Get()->num_servers(); ++i) {
    MessagePtr new_msg(new Message());
    new_msg->set_src(Zoo::Get()->rank());
    new_msg->set_dst(Zoo::Get()->server_id_to_rank(i));
    new_msg->set_type(MsgType::Load_Model);
    new_msg->set_msg_id(msg->msg_id());
    new_msg->set_table_id(msg->table_id());
    std::string new_file = file + "." + std::to_string(i);
    new_msg->Push(Blob(new_file.c_str(), new_file.length()));
    SendTo(actor::kCommunicator, new_msg);
  }
  MONITOR_END(WORKER_PROCESS_LOAD_MODEL)
}

void Worker::ProcessStoreModel(MessagePtr& msg) {
  std::string file = msg->data()[0].data();
  MONITOR_BEGIN(WORKER_PROCESS_STORE_MODEL)
  for (auto i = 0; i < Zoo::Get()->num_servers(); ++i) {
    MessagePtr new_msg(new Message());
    new_msg->set_src(Zoo::Get()->rank());
    new_msg->set_dst(Zoo::Get()->server_id_to_rank(i));
    new_msg->set_type(MsgType::Store_Model);
    new_msg->set_msg_id(msg->msg_id());
    new_msg->set_table_id(msg->table_id());
    std::string new_file = file + "." + std::to_string(i);
    new_msg->Push(Blob(new_file.c_str(), new_file.length()));
    SendTo(actor::kCommunicator, new_msg);
  }
  MONITOR_END(WORKER_PROCESS_STORE_MODEL)
}

void Worker::ProcessReplyLoadModel(MessagePtr& msg) {
  cache_[msg->table_id()]->Notify(msg->msg_id());
}

void Worker::ProcessReplyStoreModel(MessagePtr& msg) {
  cache_[msg->table_id()]->Notify(msg->msg_id());
}

}  // namespace hpps
