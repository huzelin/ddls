/*!
* \file schedule.cc
* \brief The async schedule
*/
#include "hpps/frame/scheduler/scheduler.h"

#include "hpps/common/io/io.h"
#include "hpps/frame/actor.h"
#include "hpps/frame/table_interface.h"
#include "hpps/frame/scheduler/sync_scheduler.h"

namespace hpps {

void Scheduler::ProcessGet(MessagePtr& msg) {
  if (msg->data().size() != 0) {
    MessagePtr reply(msg->CreateReplyMessage());
    server_table_->ProcessGet(msg->data(), &reply->data());
    server_->SendTo(actor::kCommunicator, reply);
  }
}

void Scheduler::ProcessAdd(MessagePtr& msg) {
  if (msg->data().size() != 0) {
    MessagePtr reply(msg->CreateReplyMessage());
    server_table_->ProcessAdd(msg->data());
    server_->SendTo(actor::kCommunicator, reply);
  }
}

void Scheduler::ProcessLoadModel(MessagePtr& msg) {
  std::string file = std::string(msg->data()[0].data(), msg->data()[0].size());
  LOG_INFO("Start loading model: %s", file.c_str());
  URI uri(file);
  auto stream = StreamFactory::GetStream(uri, FileOpenMode::BinaryRead);
  server_table_->Load(stream);
  delete stream;

  MessagePtr reply(msg->CreateReplyMessage());
  server_->SendTo(actor::kCommunicator, reply);
}

void Scheduler::ProcessStoreModel(MessagePtr& msg) {
  std::string file = std::string(msg->data()[0].data(), msg->data()[0].size());
  LOG_INFO("Start saving model: %s", file.c_str());
  URI uri(file);
  auto stream = StreamFactory::GetStream(uri, FileOpenMode::BinaryWrite);
  server_table_->Store(stream);
  delete stream;

  MessagePtr reply(msg->CreateReplyMessage());
  server_->SendTo(actor::kCommunicator, reply);
}

Scheduler* CreateScheduler(
    ServerTable* server_table, Actor* server, const std::string& ps_mode) {
  if (ps_mode == "async") {
    return new Scheduler(server_table, server);
  } else if (ps_mode == "sync") {
    return new SyncScheduler(server_table, server);
  }
  return new Scheduler(server_table, server);
}

}  // namespace hpps

