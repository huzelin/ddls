/*!
* \file schedule.cc
* \brief The async schedule
*/
#include "hpps/frame/scheduler/scheduler.h"

#include "hpps/frame/actor.h"
#include "hpps/frame/table_interface.h"

namespace hpps {

void Scheduler::ProcessGet(MessagePtr& msg) {
  if (msg->data().size() != 0) {
    MessagePtr reply(msg->CreateReplyMessage());
    server_table_->ProcessGet(msg->data(), &reply->data());
    server_actor_->SendTo(actor::kCommunicator, reply);
  }
}

void Scheduler::ProcessAdd(MessagePtr& msg) {

}

}  // namespace hpps

