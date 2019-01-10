/*!
 * \file worker.h
 * \brief The worker actor
 */
#pragma once

#include <vector>

#include "hpps/frame/actor.h"

namespace hpps {

class WorkerTable;

class Worker : public Actor {
 public:
  Worker();

  int RegisterTable(WorkerTable* worker_table);

 private:
  void ProcessGet(MessagePtr& msg);
  void ProcessAdd(MessagePtr& msg);
  void ProcessReplyGet(MessagePtr& msg);
  void ProcessReplyAdd(MessagePtr& msg);

  void ProcessLoadModel(MessagePtr& msg);
  void ProcessStoreModel(MessagePtr& msg);
  void ProcessReplyLoadModel(MessagePtr& msg);
  void ProcessReplyStoreModel(MessagePtr& msg);

  std::vector<WorkerTable*> cache_;
};

}  // namespace hpps
