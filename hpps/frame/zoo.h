/*!
 * \file zoo.h
 * \brief The ps zoo
 */
#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "hpps/frame/actor.h"
#include "hpps/frame/node.h"
#include "hpps/frame/table_interface.h"

namespace hpps {

class NetInterface;

//  Zoo Manage all components in the system, include all actors, and network
//  Maintain system information, provide method to access this information
//  Control the system, to start and end
class Zoo {
 public:
  ~Zoo();
  static Zoo* Get();

  // Start all actors
  void Start(int* argc, char** argv);
  // Stop all actors
  void Stop(bool finalize_net);

  void Barrier();

  void SendTo(const std::string& name, MessagePtr&);
  void Receive(MessagePtr& msg);

  int rank() const;
  int size() const;

  inline int worker_rank() const { return nodes_[rank()].worker_id; }
  inline int server_rank() const { return nodes_[rank()].server_id; }

  inline int rank_to_worker_id(int rank) const {
    return nodes_[rank].worker_id;
  }

  inline int rank_to_server_id(int rank) const {
    return nodes_[rank].server_id;
  }

  inline int worker_id_to_rank(int worker_id) const {
    return worker_id_to_rank_[worker_id];
  }

  inline int server_id_to_rank(int server_id) const {
    return server_id_to_rank_[server_id];
  }

  inline int num_workers() const { return num_workers_; }
  inline int num_servers() const { return num_servers_; }

  inline bool is_server() const { return node::is_server(nodes_[rank()].role); }
  inline bool is_worker() const { return node::is_worker(nodes_[rank()].role); }

  int RegisterTable(WorkerTable* worker_table);
  int RegisterTable(ServerTable* server_table);

  void RegisterActor(const std::string name, Actor* actor);

 private:
  // private constructor
  Zoo();
  void RegisterNode();
  void FinishTrain();
  void StartPS();
  void StopPS();

  std::unordered_map<std::string, Actor*> zoo_;

  std::unique_ptr<Queue<MessagePtr>> mailbox_;

  NetInterface* net_util_;

  std::vector<Node> nodes_;
  std::vector<int> server_id_to_rank_;
  std::vector<int> worker_id_to_rank_;

  int num_workers_;
  int num_servers_;

  bool inited_;
};

}  // namespace hpps
