/*!
 * \file server.h
 * \brief The server
 */
#pragma once

#include <string>
#include <vector>

#include "hpps/frame/actor.h"

namespace hpps {

class ServerTable;
class Scheduler;

class Server : public Actor {
 public:
  Server(int thread_num);
  static Server* GetServer();
  int RegisterTable(ServerTable* table);

 protected:
  virtual void ProcessGet(MessagePtr& msg);
  virtual void ProcessAdd(MessagePtr& msg);
  virtual void ProcessFinishTrain(MessagePtr& msg);

  std::vector<ServerTable*> store_;
  std::vector<std::shared_ptr<Scheduler>> scheduler_;
};

}  // namespace hpps
