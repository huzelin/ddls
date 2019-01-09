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
  explicit Scheduler(ServerTable* server_table, Actor* server) :
      server_table_(server_table), server_(server) { }

  virtual void ProcessGet(MessagePtr& msg);
  virtual void ProcessAdd(MessagePtr& msg);
  virtual void ProcessFinishTrain(MessagePtr& msg) { }

 protected:
  ServerTable* server_table_;
  Actor* server_;
};

Scheduler* CreateScheduler(ServerTable* server_table, Actor* server, const std::string& ps_mode);

}  // namespace hpps

