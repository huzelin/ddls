/*!
* \file schedule.h
* \brief The schedule
*/
#pragma once

#include <memory>

#include "hpps/common/message.h"

namespace hpps {

class ServerTable; 
class Actor;

class Scheduler {
 public:
  explicit Scheduler(ServerTable* server_table, Actor* server_actor) :
      server_table_(server_table), server_actor_(server_actor) { }

  virtual void ProcessGet(MessagePtr& msg);
  virtual void ProcessAdd(MessagePtr& msg);

 protected:
  ServerTable* server_table_;
  Actor* server_actor_;
};

}  // namespace hpps

