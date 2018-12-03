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

class Server : public Actor {
 public:
  Server();
  static Server* GetServer();
  int RegisterTable(ServerTable* table);

 protected:
  virtual void ProcessGet(MessagePtr& msg);
  virtual void ProcessAdd(MessagePtr& msg);

  std::vector<ServerTable*> store_;
};

}  // namespace hpps
