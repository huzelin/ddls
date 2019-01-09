/*!
* \file sync_schedule.cc
* \brief The sync schedule
*/
#include "hpps/frame/scheduler/sync_scheduler.h"

#include "hpps/common/log.h"
#include "hpps/frame/actor.h"
#include "hpps/frame/table_interface.h"
#include "hpps/frame/zoo.h"

namespace hpps {

// The Sync Server implement logic to support Sync SGD training
// The implementation assumes all the workers will call same number
// of Add and/or Get requests
// The server promise all workers i-th Get will get the same parameters
// If worker k has add delta to server j times when its i-th Get 
// then the server will return the parameter after all K 
// workers finished their j-th update
SyncScheduler::SyncScheduler(ServerTable* server_table, Actor* server)
      : Scheduler(server_table, server) {
  int num_worker = Zoo::Get()->num_workers();
  worker_get_clocks_.reset(new VectorClock(num_worker));
  worker_add_clocks_.reset(new VectorClock(num_worker));
  num_waited_add_.resize(num_worker, 0);
}

void SyncScheduler::ProcessAdd(MessagePtr& msg) {
  // 1. Before add: cache faster worker
  int worker = Zoo::Get()->rank_to_worker_id(msg->src());
  if (worker_get_clocks_->local_clock(worker) >
      worker_get_clocks_->global_clock()) {
    msg_add_cache_.Push(msg);
    ++num_waited_add_[worker];
    return;
  }
  // 2. Process Add
  Scheduler::ProcessAdd(msg);
  // 3. After add: process cached process get if necessary
  if (worker_add_clocks_->Update(worker)) {
    CHECK(msg_add_cache_.Empty());
    while (!msg_get_cache_.Empty()) {
      MessagePtr get_msg;
      CHECK(msg_get_cache_.TryPop(get_msg));
      int get_worker = Zoo::Get()->rank_to_worker_id(get_msg->src());
      Scheduler::ProcessGet(get_msg);
      CHECK(!worker_get_clocks_->Update(get_worker));
    }
  }
}

void SyncScheduler::ProcessGet(MessagePtr& msg) {
  // 1. Before get: cache faster worker
  int worker = Zoo::Get()->rank_to_worker_id(msg->src());
  if (worker_add_clocks_->local_clock(worker) >
      worker_add_clocks_->global_clock() || 
      num_waited_add_[worker] > 0) {
    // Will wait for other worker finished Add
    msg_get_cache_.Push(msg);
    return;
  }
  // 2. Process Get
  Scheduler::ProcessGet(msg);
  // 3. After get: process cached process add if necessary
  if (worker_get_clocks_->Update(worker)) {
    while (!msg_add_cache_.Empty()) {
      MessagePtr add_msg;
      CHECK(msg_add_cache_.TryPop(add_msg));
      int add_worker = Zoo::Get()->rank_to_worker_id(add_msg->src());
      Scheduler::ProcessAdd(add_msg);
      CHECK(!worker_add_clocks_->Update(add_worker));
      --num_waited_add_[add_worker];
    }
  }
}

void SyncScheduler::ProcessFinishTrain(MessagePtr& msg) {
  int worker = Zoo::Get()->rank_to_worker_id(msg->src());
  if (worker_add_clocks_->FinishTrain(worker)) {
    CHECK(msg_add_cache_.Empty());
    while (!msg_get_cache_.Empty()) {
      MessagePtr get_msg;
      CHECK(msg_get_cache_.TryPop(get_msg));
      int get_worker = Zoo::Get()->rank_to_worker_id(get_msg->src());
      Scheduler::ProcessGet(get_msg);
      CHECK(!worker_get_clocks_->Update(get_worker));
    }
  }
  if (worker_get_clocks_->FinishTrain(worker)) {
    CHECK(msg_get_cache_.Empty());
    while (!msg_add_cache_.Empty()) {
      MessagePtr add_msg;
      CHECK(msg_add_cache_.TryPop(add_msg));
      int add_worker = Zoo::Get()->rank_to_worker_id(add_msg->src());
      Scheduler::ProcessAdd(add_msg);
      CHECK(!worker_add_clocks_->Update(add_worker));
      --num_waited_add_[add_worker];
    }
  }
}

}  // namespace hpps
