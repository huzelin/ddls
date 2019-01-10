/*!
 * \file server.cc
 * \brief The server implementation
 */
#include "hpps/frame/server.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "hpps/frame/actor.h"
#include "hpps/common/dashboard.h"
#include "hpps/frame/table_interface.h"
#include "hpps/common/configure.h"
#include "hpps/common/queue.h"
#include "hpps/frame/zoo.h"

namespace hpps {

HPPS_DEFINE_int(server_thread_num, 1, "threadnum of server, set 10 means 10 threads");

Server::Server(int thread_num) : Actor(actor::kServer, thread_num) {
  RegisterHandler(MsgType::Request_Get, std::bind(
    &Server::ProcessGet, this, std::placeholders::_1));
  RegisterHandler(MsgType::Request_Add, std::bind(
    &Server::ProcessAdd, this, std::placeholders::_1));
  RegisterHandler(MsgType::Load_Model, std::bind(
    &Server::ProcessLoadModel, this, std::placeholders::_1));
  RegisterHandler(MsgType::Store_Model, std::bind(
    &Server::ProcessStoreModel, this, std::placeholders::_1));
  RegisterHandler(MsgType::Server_Finish_Train, std::bind(
    &Server::ProcessFinishTrain, this, std::placeholders::_1));
}

int Server::RegisterTable(ServerTable* server_table) {
  int id = static_cast<int>(store_.size());
  store_.push_back(server_table);
  std::shared_ptr<Scheduler> item(CreateScheduler(server_table, this, server_table->ps_mode()));
  scheduler_.push_back(item);
  return id;
}

void Server::ProcessGet(MessagePtr& msg) {
  MONITOR_BEGIN(SERVER_PROCESS_GET);
  int table_id = msg->table_id();
  CHECK(table_id >= 0 && table_id < static_cast<int>(store_.size()));
  scheduler_[table_id]->ProcessGet(msg);
  MONITOR_END(SERVER_PROCESS_GET);
}

void Server::ProcessAdd(MessagePtr& msg) {
  MONITOR_BEGIN(SERVER_PROCESS_ADD)
  int table_id = msg->table_id();
  CHECK(table_id >= 0 && table_id < static_cast<int>(store_.size()));
  scheduler_[table_id]->ProcessAdd(msg);
  MONITOR_END(SERVER_PROCESS_ADD)
}

void Server::ProcessLoadModel(MessagePtr& msg) {
  MONITOR_BEGIN(SERVER_LOAD_MODEL)
  int table_id = msg->table_id();
  CHECK(table_id >= 0 && table_id < static_cast<int>(store_.size()));
  scheduler_[table_id]->ProcessLoadModel(msg);
  MONITOR_END(SERVER_LOAD_MODEL)
}

void Server::ProcessStoreModel(MessagePtr& msg) {
  MONITOR_BEGIN(SERVER_STORE_MODEL)
  int table_id = msg->table_id();
  CHECK(table_id >= 0 && table_id < static_cast<int>(store_.size()));
  scheduler_[table_id]->ProcessStoreModel(msg);
  MONITOR_END(SERVER_STORE_MODEL)
}

void Server::ProcessFinishTrain(MessagePtr& msg) {
  int table_id = msg->table_id();
  CHECK(table_id >= 0 && table_id < static_cast<int>(store_.size()));
  scheduler_[table_id]->ProcessFinishTrain(msg);
}

Server* Server::GetServer() {
  return new Server(HPPS_CONFIG_server_thread_num);
}

}  // namespace hpps
